#include <stdio.h>
#include <stdlib.h>
#include "ext2_fs.h"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
int block_size;
void handleError() {
  fprintf(stderr, "Error: %s\n", strerror(errno));
  exit(2);
}

void printDirectoryEntries(int fd, int inodeNumber, struct ext2_inode* inode) {
  for(int i = 0; i < EXT2_NDIR_BLOCKS; i++) {
    if((*inode).i_block[i] != 0) {
      int offset = (*inode).i_block[i]*block_size;
      int nOffset = offset + block_size;
      int pOffset = offset;
      struct ext2_dir_entry dir;
      while(pOffset < nOffset) {
	if(pread(fd, &dir, sizeof(dir), pOffset) < 0) { handleError(); }
	if(dir.inode != 0) {
	  printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n", inodeNumber, pOffset - offset, dir.inode, dir.rec_len, dir.name_len, dir.name);
	}
	pOffset += dir.rec_len;
      }
    }
  }
}

void printIndirectBlockReferences(int fd, int blockNum, int level, int inodeNum, int offset, struct ext2_inode* inode) {
  char fileType = '?';
  switch((*inode).i_mode & 0xF000) {
  case 0x8000:
    fileType = 'f'; break;
  case 0x4000:
    fileType = 'd'; break;
  case 0xA000:
    fileType = 's'; break;
  default: break;
  }
  int indirectBlocks[block_size/4];
  if(pread(fd, &indirectBlocks, block_size, blockNum*block_size) < 0) { handleError(); }
  for(int i = 0; i < block_size/4; i++) {
    if(indirectBlocks[i] != 0) {
      printf("INDIRECT,%d,%d,%d,%d,%d\n", inodeNum, level, offset + i, blockNum, indirectBlocks[i]);
      if(level > 1)
          printIndirectBlockReferences(fd, indirectBlocks[i], level-1, inodeNum, offset + i, inode);
        if (level == 1 && fileType == 'd') {
             struct ext2_dir_entry dir;
                    int offset =  indirectBlocks[i]*block_size;
                    int nOffset = offset + block_size;
                    int pOffset = offset;
                    while(pOffset < nOffset) {
                        if(pread(fd, &dir, sizeof(dir), pOffset) < 0) { handleError(); }
                        if(dir.inode != 0) {
                            printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n", inodeNum, pOffset - offset, dir.inode, dir.rec_len, dir.name_len, dir.name);
                        }
                        pOffset += dir.rec_len;
                    }
        }
        
    }
  }
}

void printFreeEntries(int fd, int groupNum, int blocksize, struct ext2_super_block* super, int bitmap, char *entryType) {
  char buffer;
  for(int i = 0; i < blocksize; i++) {
    if(pread(fd, &buffer, 1, bitmap*blocksize + i) < 0) { handleError(); }
    for(int j = 0; j < 8; j++) {
      if((buffer & (1 << j)) == 0) {
	printf("%s,%u\n", entryType, (i*8)+j+1+((*super).s_blocks_per_group*groupNum));
      }
    }
  }
}

void printInodeSummary(int fd, int inodeTable, int numInodes) {
  struct ext2_inode inodes[numInodes];
  if(pread(fd, inodes, sizeof(inodes), (block_size*inodeTable)) < 0) { handleError(); }
  for(int i = 0; i < numInodes; i++) {
    if(inodes[i].i_mode != 0 && inodes[i].i_links_count != 0) {
      char fileType = '?';
      switch(inodes[i].i_mode & 0xF000) {
      case 0x8000:
	fileType = 'f'; break;
      case 0x4000:
	fileType = 'd'; break;
      case 0xA000:
	fileType = 's'; break;
      default: break;
      }
      printf("INODE,%d,%c,%o,%d,%d,%d,", i+1, fileType, (inodes[i].i_mode & 0xFFF), inodes[i].i_uid, inodes[i].i_gid, inodes[i].i_links_count);
      for(int t = 0; t<3; t++) {
	time_t time;
	char tempString[20];
	switch(t) {
	case 0: time = inodes[i].i_ctime; break;
	case 1: time = inodes[i].i_mtime; break;
	case 2: time = inodes[i].i_atime; break;
	default: break;
	}
	struct tm *gm = gmtime(&time);
	strftime(tempString, 18, "%m/%d/%y %H:%M:%S", gm);
	printf("%s,", tempString);
      }
      printf("%d,%d", inodes[i].i_size, inodes[i].i_blocks);
      for(int b = 0; b < EXT2_N_BLOCKS; b++) {
	printf(",%d", inodes[i].i_block[b]);
      }
      printf("\n");
      if(fileType == 'd')
	printDirectoryEntries(fd, i+1, &inodes[i]);
      printIndirectBlockReferences(fd, inodes[i].i_block[EXT2_IND_BLOCK], 1, i+1, 12, &inodes[i]);
      printIndirectBlockReferences(fd, inodes[i].i_block[EXT2_DIND_BLOCK], 2, i+1, 268, &inodes[i]);
      printIndirectBlockReferences(fd, inodes[i].i_block[EXT2_TIND_BLOCK], 3, i+1, 268 + ((block_size/4.0) * (block_size/4.0)), &inodes[i]);
    }
  }
}

int main(int argc, char *argv[]) {
  if(argc != 2) {
    fprintf(stderr, "Error! must require 1 argument\n");
    exit(1);
  }
  int fileDescriptor;
  if((fileDescriptor = open(argv[1], O_RDONLY)) < 0) {
    fprintf(stderr, "Error: open file failed \n");
    exit(1);
  }
  
  //SUPER BLOCK
  struct ext2_super_block super_block;
  if(pread(fileDescriptor, &super_block, sizeof(super_block), 1024) < 0) {
    fprintf(stderr, "Error: %s", strerror(errno));
    exit(2);
  }
  block_size = 1024 << super_block.s_log_block_size;
  printf("SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n", super_block.s_blocks_count, super_block.s_inodes_count, block_size, super_block.s_inode_size, super_block.s_blocks_per_group, super_block.s_inodes_per_group, super_block.s_first_ino);
  
  //GROUP
  int numGroups = (super_block.s_blocks_count/super_block.s_blocks_per_group);
  if((super_block.s_blocks_count % super_block.s_blocks_per_group) != 0)
    numGroups++;
  struct ext2_group_desc groups[numGroups];
  if(pread(fileDescriptor, groups, numGroups*sizeof(struct ext2_group_desc),  (super_block.s_first_data_block +1) * block_size ) < 0) {
    fprintf(stderr, "Error reading groups!\n");
    exit(2);
  }
  int inodesPerGroup = super_block.s_inodes_count % super_block.s_inodes_per_group;
  if(inodesPerGroup == 0)
    inodesPerGroup = super_block.s_inodes_per_group;
  int numInodes = 0;
  for(int i = 0; i<numGroups; i++) {
    int blocksPerGroup = 0;
    if(i == numGroups-1) {
      blocksPerGroup = super_block.s_blocks_count - (super_block.s_blocks_per_group*i);
    } else {
      blocksPerGroup = super_block.s_blocks_per_group;
    }
    printf("GROUP,%u,%u,%u,%u,%u,%u,%u,%u\n", i, blocksPerGroup, inodesPerGroup, groups[i].bg_free_blocks_count, groups[i].bg_free_inodes_count, groups[i].bg_block_bitmap, groups[i].bg_inode_bitmap, groups[i].bg_inode_table);
    printFreeEntries(fileDescriptor, i, block_size, &super_block, groups[i].bg_block_bitmap, "BFREE");
    printFreeEntries(fileDescriptor, i, block_size, &super_block, groups[i].bg_inode_bitmap, "IFREE");
    if(i == numGroups-1) {
      numInodes = super_block.s_inodes_count - (super_block.s_inodes_per_group*i);
    } else {
      numInodes = super_block.s_inodes_per_group;
    }
    printInodeSummary(fileDescriptor, groups[i].bg_inode_table, numInodes);
  }
}
