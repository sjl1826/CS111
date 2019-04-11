#!/usr/local/bin/python2.7

import sys

referencedBlocks = {}
INODE_OFFSET_STR = " {} IN INODE {} AT OFFSET {}"

class IndirectBlock:
    def __init__(self, inode_number, level, offset, block, ref_block):
        self.inodeNumber = int(inode_number)
        self.level = int(level)
        self.offset = int(offset)
        self.block = int(block)
        self.refBlock = int(ref_block)

    def checkValidBlock(self, blockNum, numBlocks, freeBlocks, inconsistencyType):
        if blockNum == 0:
            return
        if blockNum < 0 or blockNum > numBlocks - 1:
            print("INVALID " + inconsistencyType + INODE_OFFSET_STR.format(blockNum, self.inodeNumber, self.offset))
        elif blockNum < 8:
            print("RESERVED " + inconsistencyType + INODE_OFFSET_STR.format(blockNum, self.inodeNumber, self.offset))
        elif blockNum in freeBlocks:
            print("ALLOCATED BLOCK {} ON FREELIST".format(blockNum))
        elif blockNum in referencedBlocks:
            referencedBlocks[blockNum].append((self, self.offset, inconsistencyType))
        else:
            referencedBlocks[blockNum] = [(self, self.offset, inconsistencyType)]

def processIndirectBlock(indirectBlock):
    return IndirectBlock(indirectBlock[1], indirectBlock[2], indirectBlock[3], indirectBlock[4], indirectBlock[5])

def auditIndirectBlock(indirectBlocks, numBlocks, freeBlocks):
    blockLevel = ""
    for indirectBlock in indirectBlocks:
        if indirectBlock.level == 1: blockLevel = "BLOCK"
        elif indirectBlock.level == 2: blockLevel = "INDIRECT BLOCK"
        elif indirectBlock.level == 3: blockLevel = "DOUBLE INDIRECT BLOCK"
        indirectBlock.checkValidBlock(indirectBlock.refBlock, numBlocks, freeBlocks, blockLevel)


class Inode:
    def __init__(self, inode_number, file_type, block_pointers, indirect_block, double_indirect_block, triple_indirect_block):
        self.inodeNumber = int(inode_number)
        self.fileType = file_type
        self.blockPointers = block_pointers
        self.indirect = int(indirect_block)
        self.doubleIndirect = int(double_indirect_block)
        self.tripleIndirect = int(triple_indirect_block)

    # Reusable check for blocks, private to class
    def __checkValidBlock(self, blockNum, numBlocks, freeBlocks, inconsistencyType, offset):
        if blockNum == 0:
            return
        if blockNum < 0 or blockNum > numBlocks - 1:
            print("INVALID " + inconsistencyType + INODE_OFFSET_STR.format(blockNum, self.inodeNumber, offset))
        elif blockNum < 8:
            print("RESERVED " + inconsistencyType + INODE_OFFSET_STR.format(blockNum, self.inodeNumber, offset))
        elif blockNum in freeBlocks:
            print("ALLOCATED BLOCK {} ON FREELIST".format(blockNum))
        elif blockNum in referencedBlocks:
            referencedBlocks[blockNum].append((self, offset, inconsistencyType))
        else:
            referencedBlocks[blockNum] = [(self, offset, inconsistencyType)]
    
    def checkValidBlocks(self, numBlocks, freeBlocks):
        offset = 0
        for block in self.blockPointers:
            self.__checkValidBlock(int(block), numBlocks, freeBlocks, "BLOCK", offset)
            offset += 1
        self.__checkValidBlock(self.indirect, numBlocks, freeBlocks, "INDIRECT BLOCK", 12)
        self.__checkValidBlock(self.doubleIndirect, numBlocks, freeBlocks, "DOUBLE INDIRECT BLOCK", 268)
        self.__checkValidBlock(self.tripleIndirect, numBlocks, freeBlocks, "TRIPLE INDIRECT BLOCK", 65804)

# Takes an array split up by commas and returns a properly created Inode
def processInode(inode):
    inodeNumber = inode[1]
    fileType = inode[2]
    blockPointers = []
    # Symbolic links do not contain block numbers and should not be analyzed
    if fileType != 's':
        # Create an array of block pointers
        for pointer in inode[12:24]:
            blockPointers.append(pointer)
        return Inode(inodeNumber, fileType, blockPointers, inode[24], inode[25], inode[26])

def auditBlockConsistency(inodes, numBlocks, freeBlocks):
    for inode in inodes:
        if inode.fileType != 's':
            inode.checkValidBlocks(numBlocks, freeBlocks)

def main():
    inodes = []
    freeBlocks = []
    indirectBlocks = []

    # Information from the super block
    numBlocks = 0
    numInodes = 0
    firstNonReservedInode = 0

    if len(sys.argv) != 2:
        sys.stderr.write("Error: invalid number of arguments!\n")
        sys.exit(1)
    data = []
    
    try:
        with open(sys.argv[1]) as file:
            for line in file:
                data.append(line)
    except IOError:
        sys.stderr.write("Error: could not read file \n")
        sys.exit(1)

    # Loop through each csv line
    for line in data:
        # Split line into list
        tempArray = line.split(',')
        type = tempArray[0]
        if type == "INODE":
            inodes.append(processInode(tempArray))
        elif type == "SUPERBLOCK":
            numBlocks = int(tempArray[1])
            numInodes = int(tempArray[2])
            firstNonReservedInode = int(tempArray[7])
        elif type == "BFREE":
            freeBlocks.append(int(tempArray[1]))
        elif type == "INDIRECT":
            indirectBlocks.append(processIndirectBlock(tempArray))

    auditBlockConsistency(inodes, numBlocks, freeBlocks)
    auditIndirectBlock(indirectBlocks, numBlocks, freeBlocks)

    for blockNumber in range(8, numBlocks):
        if blockNumber not in referencedBlocks and blockNumber not in freeBlocks:
            print("UNREFERENCED BLOCK {}".format(blockNumber))
        if blockNumber in referencedBlocks:
            if len(referencedBlocks[blockNumber]) > 1:
                for duplicateBlock in referencedBlocks[blockNumber]:
                    print("DUPLICATE {}".format(duplicateBlock[2]) + INODE_OFFSET_STR.format(blockNumber, duplicateBlock[0].inodeNumber, duplicateBlock[1]))

if __name__ == "__main__":
    main()
