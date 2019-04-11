
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
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

struct timeval timeA;
int isLogging = 0;
int logfile = -1;
int stopReports = 0;
int sockfd = -1;

void shut_down() {
    gettimeofday(&timeA, NULL);
    struct tm *l_time = localtime(&(timeA.tv_sec));
    /*char out_text[30];
    sprintf(out_text, "%02d:%02d:%02d SHUTDOWN\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec);
    write(sockfd, out_text, 18);*/
    dprintf(sockfd, "%02d:%02d:%02d SHUTDOWN\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec);
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
        {"id", required_argument, 0, 'i'},
        {"host", required_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0, opt;
    int sampling_interval = 1;
    char temperature_scale = 'F';
    char * id_num;
    int id_flag = 0;
    char * host;
    int host_flag = 0;
    struct hostent *server;
    int port_number = -1;

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
                    if((logfile = creat(optarg, 0666)) < 0) {
                        fprintf(stderr, "ERROR: File could not be created.\n");
                        exit(1);
                    }
                    isLogging = 1;
                    break;
                case 'i':
                    id_num = optarg;
                    if(strlen(id_num) != 9) {
                        fprintf(stderr, "Error: ID Number must be 9 characters long\n");
                        exit(1);
                    }
                    id_flag = 1;
                    break;
                case 'h':
                    host = optarg;
                    host_flag = 1;
                    break;
                default:
                    fprintf(stderr, "Invalid argument given: %s\n", strerror(errno));
                    exit(1);
                    break;
            }
        } else if(optind < argc) {
            port_number = atoi(argv[optind]);
            break;
        } else {
            fprintf(stderr, "Error, no port number\n");
            exit(1);
        }
    }

    if(port_number <= 1024) {
        fprintf(stderr, "Error: port number must be provided\n");
        exit(1);
    }

    if(!isLogging || !host_flag || !id_flag) {
        fprintf(stderr, "Error: must provide id, host, and log\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        fprintf(stderr, "Error opening socket\n");
        exit(1);
    }

    server = gethostbyname(host);
    if(server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port_number);

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Error: could not connect\n");
        exit(1);
    }

    /*char id_text[20];
    sprintf(id_text, "ID=%s\n", id_num);
    write(sockfd, id_text, 13);*/
    dprintf(sockfd, "ID=%s\n", id_num);
    if(isLogging)
        dprintf(logfile, "ID=%s\n", id_num);

    // Initialize sensors
    mraa_aio_context temperature_sensor = mraa_aio_init(1);
    if(temperature_sensor == NULL) {
        fprintf(stderr, "Error: Temperature sensor could not be initialized\n");
        exit(1);
    }

    struct pollfd poll_fd;

    poll_fd.events = (POLLIN | POLLERR | POLLHUP);
    poll_fd.fd = sockfd;

    float temperature;
    time_t totalPTime = 0;

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
                //char out_text[20];
                //sprintf(out_text, "%02d:%02d:%02d %.1f\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec, temperature);
                //write(sockfd, out_text, 14);
                dprintf(sockfd, "%02d:%02d:%02d %.1f\n", l_time->tm_hour, l_time->tm_min, l_time->tm_sec, temperature);
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
            exit(1);
        } else {
            if(poll_fd.revents & POLLIN) {
                char buffer[256];
                bzero((char*)buffer, 256);
                char command[100];
                ssize_t bytes = read(poll_fd.fd, buffer, 256);
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
                            shut_down();
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
    return 0;
}


