// ptp2bin - Covert the peculiar ITS CROSS assembler PTP file to binary
//
// Released under the MIT License.  See accompanying LICENSE file.
// Copyright 2018 Peter Neubauer

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdint>

int readOne(std::ifstream& input, std::ofstream& output, uint16_t start, uint16_t lengthLimit)
{
    // Scan for the line-starting delimiter.
    char delimit = ' ';
    while (delimit != ';')
        input >> std::setw(1) >> delimit;
    
    // Parse the one byte record length (in bytes) and the 2 byte load address.
    // All values are in hex.
    std::string lengthHex, addrHex;
    input >> std::setw(2) >> lengthHex >> std::setw(4) >> addrHex;
    
    const uint16_t length = std::stoi(lengthHex, nullptr, 16);
    uint16_t addr = std::stoi(addrHex, nullptr, 16);

    // If the record length is 0, there's no more data.
    if (length == 0)
        return 0;
    
    // Process each byte of data.
    //std::cout << std::hex << std::uppercase << std::setw(4) << addr << ":";
    for (int i = 0; i < length; i++)
    {
        std::string dataHex;
        input >> std::setw(2) >> dataHex;
        const uint8_t data = std::stoi(dataHex, nullptr, 16);
        
        //std::cout << " " << std::hex << std::uppercase << std::setw(2) << dataHex;
        
        // Discard data outside to user-specified [start, start+length] range.
        if ((addr >= start) && (addr < (start + lengthLimit)))
        {
            // Otherwise, write the data.  Beware that the PTP file may not be contiguous or in order.
            output.seekp(addr - start);
            output.write((const char*)&data, 1);
        }
        addr++;
    }
    
    //std::cout << std::endl;
    
    return length;
}

int main(int argc, char* argv[])
{
    // If the user passes an incorrect parameter count, print usage.
	if (argc != 5)
	{
		std::cerr << "Usage: ptp2bin start length filename.ptp outfile" << std::endl;
        std::cerr << "  start        - load address of the binary in hex" << std::endl;
        std::cerr << "  length       - length of the binary in hex" << std::endl;
        std::cerr << "  filename.ptp - input file in the peculiar PTP format from the ITS CROSS assember" << std::endl;
        std::cerr << "  outfile      - output file to contain the PTP input converted to binary" "" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Convert a PTP file into a binary file suitable for transfer to an Apple II disk image." << std::endl;
        std::cerr << "Since a PTP file may not be contiguous and may patch system software, this utility" << std::endl;
        std::cerr << "requires a 'start' and 'length', expressed in hex, of the desired converted binary." << std::endl;
        std::cerr << "This utility zero discards data in the PTP outside this range and zero pads if needed." << std::endl;
        std::cerr << "Successful use requires knowledge of the original program's memory layout." << std::endl;
		return -1;
	}
    
    // Parse parameters.  Error handling is lacking!
    const std::string startAddrHex = argv[1];
    uint16_t start = std::stoi(startAddrHex, nullptr, 16);
    
    const std::string lengthLimitHex = argv[2];
    uint16_t lengthLimit = std::stoi(lengthLimitHex, nullptr, 16);
    
    const std::string inputFile = argv[3];
    std::ifstream input(inputFile);
    
    const std::string outputFile = argv[4];
    std::ofstream output(outputFile, std::ios::binary | std::ios::out | std::ios::trunc);
    
    // Force the file to the user-specified length.
    output.seekp(lengthLimit);
    output.seekp(0);

    // Process each line of the input PTP file.  The file ends with a record containing a byte count of 0.
    uint16_t totalLength = 0, recordLength;
    do
    {
        recordLength = readOne(input, output, start, lengthLimit);
        totalLength += recordLength;
    } while (recordLength > 0);
    
    // Print summaries.
    std::cout << "Translated " << std::hex << totalLength << " bytes." << std::endl;
    std::cout << "start=0x" << std::hex << std::setw(4) << start << " length=0x" << std::hex << std::setw(4) << lengthLimit << std::endl;
    
	return 0;
}
