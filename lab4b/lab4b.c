#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <mraa.h>
#include <mraa/aio.h>
#include <sys/time.h>
#include <math.h>

struct timeval timeA;
int isLogging = 0;
int logfile = -1;
int stopReports = 0;

void shutdown() {
    gettimeofday(&timeA, NULL);
    struct tm *l_time = localtime(&(timeA.tv_sec));
    fprintf(stdout, "%02d:%02d:%02d SHUTDOWN\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec);
    if(isLogging) {
        dprintf(logfile, "%02d:%02d:%02d SHUTDOWN\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec);
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"period", required_argument, 0, 'p'},
        {"scale", required_argument, 0, 's'},
        {"log", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };

    int option_index = 0, opt;
    int sampling_interval = 1;
    char temperature_scale = 'F';

    while(optind < argc) {
        if((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
            switch(opt) {
                case 'p':
                    sampling_interval = atoi(optarg);
                    break;
                case 's':
                    switch(optarg[0]) {
                        case 'F':
                        case 'C':
                            temperature_scale = optarg[0];
                            break;
                        default:
                            fprintf(stderr, "Invalid Temperature Scale (Use F or C)\n");
                            exit(1);
                            break;
                    }
                    break;
                case 'l':
                    isLogging = 1;
                    if((logfile = creat(optarg, 0666)) < 0) {
                        fprintf(stderr, "ERROR: File could not be created.\n");
                        exit(1);
                    }
                    break;
                default:
                    fprintf(stderr, "Invalid argument given: %s\n", strerror(errno));
                    exit(1);
                    break;
            }
        }
    }

    // Initialize sensors
    mraa_aio_context temperature_sensor = mraa_aio_init(1);
    if(temperature_sensor == NULL) {
        fprintf(stderr, "Error: Temperature sensor could not be initialized\n");
        exit(1);
    }
    mraa_gpio_context button;
    button = mraa_gpio_init(60);
    if(button == NULL) {
        fprintf(stderr, "Error: Button could not be initialized\n");
        exit(1);
    }
    struct pollfd poll_fd;
    poll_fd.events = (POLLIN | POLLERR | POLLHUP);
    poll_fd.fd = STDIN_FILENO;

    float temperature;
    time_t totalPTime = 0;

    mraa_gpio_dir(button, MRAA_GPIO_IN);
    mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &shutdown, NULL);

    while(1) {
        gettimeofday(&timeA, NULL);
        if((timeA.tv_sec-totalPTime) >= sampling_interval) {
            double temp;
            if((temp = mraa_aio_read(temperature_sensor)) < 0) {
                fprintf(stderr, "ERROR: Temperature read failed\n");
                exit(1);
            }
            float R = 1023.0/temp-1.0;
            const int R0 = 100000;
            const int B = 4275;
            R = R0*R;
            temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;
            if(temperature_scale == 'F') {
                temperature = temperature*1.8 + 32;
            }
            struct tm *l_time = localtime(&(timeA.tv_sec));
            if(!stopReports) {
                fprintf(stdout, "%02d:%02d:%02d %.1f\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec, temperature);
            }
            if(isLogging) {
                dprintf(logfile, "%02d:%02d:%02d %.1f\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec, temperature);
            }
            totalPTime = timeA.tv_sec;
        }

        int pollVal;
        if ((pollVal = poll(&poll_fd, 1, 0)) < 0) {
            fprintf(stderr, "ERROR: %s\n", strerror(errno));
            mraa_aio_close(temperature_sensor);
            mraa_gpio_close(button);
            exit(1);
        } else {
            if(poll_fd.revents & POLLIN) {
                char buffer[256];
                bzero((char*)buffer, 256);
                char command[100];
                ssize_t bytes = read(STDIN_FILENO, buffer, 256);
                int i = 0;
                int j = 0;
                for(i = 0; i < bytes; i++) {
                    if(buffer[i] == '\n') {
                        command[j] = '\0';
                        j = 0;
                        if(isLogging)
                            dprintf(logfile, "%s\n", command);
                        if(!strcmp(command, "SCALE=F")) {
                            temperature_scale = 'F';
                        } else if(!strcmp(command, "SCALE=C")) {
                            temperature_scale = 'C';
                        } else if(!strncmp(command, "PERIOD=", 7)) {
                            sampling_interval = atoi(&command[7]);
                        } else if(!strcmp(command, "STOP")) {
                            stopReports = 1;
                        } else if(!strcmp(command, "START")) {
                            stopReports = 0;
                        } else if(!strcmp(command, "OFF")) {
                            shutdown();
                        }
                        memset(command, 0, bytes);
                    } else {
                        command[j] = buffer[i];
                        j++;
                    }
                }
            }
        }
    }
    mraa_aio_close(temperature_sensor);
    mraa_gpio_close(button);
    return 0;
}

