//-----------------------------------------------------------------------
// VERSION 1.0.0
// FILE: main.cpp
// OBJECT: main
// DESCRIPTION: TagDataReader
//     Read data from the file and write another file according to the tag file.
//     Files should locate in same folder with the program.
//     Input: inputfile defined with --input flag ang tagfile defined with --tagfile flag
//     Output: outputfile defined with --output flag
//     Required command line arguments:
//         --tagfile <filaname>
//         --input <filename>
//         --output <filename>
//     Optional command line arguments in this order:
//         row_length           number default 8192
//         tag_file length      number default 1024
//         tag length           number default 50
//         column max amount    number default 50
//         debug when 1         number default 0
//         debug2 when 1        number default 0
//     Tag file examples:
//         To read elements present only once or missing
//             <person,0
//             identifier=",1
//             identifier2=",1
//             <name>,1
//             <address>,1
//             <gender>,1
//             <birthdate>,1
//             <phone>,1
//             <education>,1
//         To read elements present many times
//             <person,0
//             identifier=",1
//             identifier2=",1
//             <workhistory>,2
//         From XML file like
//             <person identifier="12345" identifier2="54321">
//             <name>John</name>
//             <address>Dallas</address>
//             <gender>male</gender>
//             <birthdate>10.11.1964</birthdate>
//             <phone>555-555</phone>
//             <education>PhD</education>
//             <workhistory>trainee at company A</workhistory>
//             <workhistory>clerk at company B</workhistory>
//             <workhistory>manager at company C</workhistory>
//        Then special cases in tag file
//          Convert with
//             <name,1,c
//          For
//             <name nickname="Jhonny"><firstname>John</firstname><middlename>Miles</middlename>Smith</name>
//           See content when testing with 
//             <name,1
//           Get only Smith with 
//             <name,1,l
//           Get attrbutes and Smith with 
//             <name,1,o
//           Get attributes and Smith and child elements
//             <name,1,o
//             <firstname>,1
//             <middlename>,1
//           Also works
//             <name,1,l
//             <nickname=",1
//             <firstname>,1
//             <middlename>,1
// 
//
// CREATED: 27.5.2021
// MODIFICATION HISTORY:
// ...
// 2021-06-08 Harri Kalikka Test version
// 2021-05-27 Harri Kalikka Modification for g++
//-----------------------------------------------------------------------
// Copyright 2021 Harri Kalikka
//-----------------------------------------------------------------------
#include <iostream>
#include <cstring>
#include <cmath>
#include "dbios.h"

// Convert attributes and data of tag element to csv format
// Used when tag is marked with ,o and then attributes and only data of tag is writen.
long write_array_outer(char *out_data, char *data, long &out_count, long pos_tag, long pos_end, long taglen, long debug)
{
    long start_count = out_count;
    long ret_val = 0;
    long i = 0; 
    long j = 0;
    long k = 0; 
    long l = 0;
    long checked = 0;
    char left = '|';
    char right = '|';
    long wstart = 0;
    long wend = 0;
    bool is_write = false;
    bool is_set = false;
    long z = 0;
    bool is_found = false;

    if (debug) {
        std::cout << "pos_tag: " << pos_tag << std::endl;
        std::cout << "pos_end:: " << pos_end << std::endl;
        std::cout << "out_count: " << out_count << std::endl;
        std::cout << "taglen: " << taglen << std::endl;
    }

    // Loop all characterss between start and end tags
    while (checked < (pos_end - pos_tag - taglen)) {

        left = '|';
        right = '|';
        // Note that l remember last round
        for (k = 0; l < (pos_end - pos_tag - taglen) && !is_write; k++, l++, is_set = false) {
            if (data[pos_tag + 1 + l] == '"' && data[pos_tag + l] == '=' && left == '|' && right == '|' && !is_set) {
                left = '"';
                wstart = (pos_tag + 2 + l);
                is_set = true;
            }
            if (data[pos_tag + 2 + l] == '"' && left == '"' && right == '|' && !is_set) {
                right = data[pos_tag + 1 + l];
                wend = (pos_tag + 1 + l);
                is_set = true;
                z = wstart;
                while (z <= wend) {
                    if (data[z] != ' ' && data[z] != '\n') {
                        is_write = true;
                    }
                    z++;
                }
                if (!is_write) {
                    left = '|';
                    right = '|';
                    wstart = wend;
                }            
            }
            if (debug) {
                std::cout << "check char: " << (pos_tag + 1 + l) << std::endl;
                std::cout << "check char: " << data[pos_tag + 1 + l] << std::endl;
                std::cout << "end value: " << (pos_end - pos_tag - taglen) << std::endl;
                std::cout << "current value: " << l << std::endl;
                // In debug limit data of tag to 201 char
                if (l > 200) {
                    break;
                }
            }
        }
        checked = l;

        if (debug) {
            std::cout << "checked: " << checked << std::endl;
            std::cout << "left: " << left << std::endl;
            std::cout << "rigth: " << right << std::endl;
            std::cout << "is_write: " << is_write << std::endl;
            std::cout << "watart: " << wstart << std::endl;
            std::cout << "wend: " << wend << std::endl;
        }

        if (is_write) {

            if (debug) {
                std::cout << "start write to array" << std::endl;
            }
            // Comma separator
            if (out_count > 0) {
                out_data[out_count] = (char) 0x2c;
                out_count++; 
            }

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 

            // Data of tag element
            for (i = out_count, j = wstart; j <= wend; i++, j++) {
                    out_data[i] = data[j];
            }
            out_count = i;

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 
        }

        if (debug) {
            std::cout << "wrote until to char in out array: " << out_count << std::endl;
        }
        is_write = false;
        ret_val = out_count - start_count;
    }

    // Used similar while loop again then initialize variables
    i = 0; 
    j = 0;
    k = 0; 
    checked = 0;
    left = '|';
    right = '|';
    wstart = 0;
    wend = 0;
    is_write = false;
    is_set = false;
    z = 0;
    is_found = false;

    l = pos_end;

    // Loop all characterss between start and end tags
    while (checked < (pos_end - pos_tag - taglen) && !is_found) {

        left = '|';
        right = '|';
        // Note that l remember last round
        for (k = 0; l >= pos_tag && !is_write; k++, l--, is_set = false) {
            if (data[l] == '<' && left == '|' && right == '|' && !is_set) {
                right = '<';
                wend = l - 1;
                is_set = true;
            }
            if (data[l] == '>' && left == '|' && right == '<' && !is_set) {
                left = '>';
                wstart = l + 1;
                is_set = true;
                z = wstart;
                while (z <= wend) {
                    if (data[z] != ' ' && data[z] != '\n') {
                        is_write = true;
                        is_found = true;
                    }
                    z++;
                }
                if (!is_write) {
                    left = '|';
                    right = '|';
                    wend = wstart;
                }            
            }
            if (debug) {
                std::cout << "check char: " << (pos_tag + 1 + l) << std::endl;
                std::cout << "check char: " << data[pos_tag + 1 + l] << std::endl;
                std::cout << "end value: " << (pos_end - pos_tag - taglen) << std::endl;
                std::cout << "current value: " << l << std::endl;
                // In debug limit data of tag to 201 char
                if (k > 200) {
                    break;
                }
            }
        }
        checked = checked + k;

        if (debug) {
            std::cout << "checked: " << checked << std::endl;
            std::cout << "left: " << left << std::endl;
            std::cout << "rigth: " << right << std::endl;
            std::cout << "is_write: " << is_write << std::endl;
            std::cout << "watart: " << wstart << std::endl;
            std::cout << "wend: " << wend << std::endl;
        }

        if (is_write) {

            if (debug) {
                std::cout << "start write to array" << std::endl;
            }
            // Comma separator
            if (out_count > 0) {
                out_data[out_count] = (char) 0x2c;
                out_count++; 
            }

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 

            // Data of tag element
            for (i = out_count, j = wstart; j <= wend; i++, j++) {
                    out_data[i] = data[j];
            }
            out_count = i;

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 
        }

        if (debug) {
            std::cout << "wrote until to char in out array: " << out_count << std::endl;
        }
        is_write = false;
        ret_val = out_count - start_count;
    }

    return(ret_val);
}

// Convert data of tag element to csv format
// Used when tag is marked with ,l and then only data of tag is writen. read backwards and write forward
long write_array_last(char *out_data, char *data, long &out_count, long pos_tag, long pos_end, long taglen, long debug)
{
    long start_count = out_count;
    long ret_val = 0;
    long i = 0; 
    long j = 0;
    long k = 0; 
    long l = 0;
    long checked = 0;
    char left = '|';
    char right = '|';
    long wstart = 0;
    long wend = 0;
    bool is_write = false;
    bool is_set = false;
    long z = 0;
    bool is_found = false;

    if (debug) {
        std::cout << "pos_tag: " << pos_tag << std::endl;
        std::cout << "pos_end:: " << pos_end << std::endl;
        std::cout << "out_count: " << out_count << std::endl;
        std::cout << "taglen: " << taglen << std::endl;
    }

    l = pos_end;

    // Loop all characterss between start and end tags
    while (checked < (pos_end - pos_tag - taglen) && !is_found) {

        left = '|';
        right = '|';
        // Note that l remember last round
        for (k = 0; l >= pos_tag && !is_write; k++, l--, is_set = false) {
            if (data[l] == '<' && left == '|' && right == '|' && !is_set) {
                right = '<';
                wend = l - 1;
                is_set = true;
            }
            if (data[l] == '>' && left == '|' && right == '<' && !is_set) {
                left = '>';
                wstart = l + 1;
                is_set = true;
                z = wstart;
                while (z <= wend) {
                    if (data[z] != ' ' && data[z] != '\n') {
                        is_write = true;
                        is_found = true;
                    }
                    z++;
                }
                if (!is_write) {
                    left = '|';
                    right = '|';
                    wend = wstart;
                }            
            }
            if (debug) {
                std::cout << "check char: " << (pos_tag + 1 + l) << std::endl;
                std::cout << "check char: " << data[pos_tag + 1 + l] << std::endl;
                std::cout << "end value: " << (pos_end - pos_tag - taglen) << std::endl;
                std::cout << "current value: " << l << std::endl;
                // In debug limit data of tag to 201 char
                if (k > 200) {
                    break;
                }
            }
        }
        checked = checked + k;

        if (debug) {
            std::cout << "checked: " << checked << std::endl;
            std::cout << "left: " << left << std::endl;
            std::cout << "rigth: " << right << std::endl;
            std::cout << "is_write: " << is_write << std::endl;
            std::cout << "watart: " << wstart << std::endl;
            std::cout << "wend: " << wend << std::endl;
        }

        if (is_write) {

            if (debug) {
                std::cout << "start write to array" << std::endl;
            }
            // Comma separator
            if (out_count > 0) {
                out_data[out_count] = (char) 0x2c;
                out_count++; 
            }

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 

            // Data of tag element
            for (i = out_count, j = wstart; j <= wend; i++, j++) {
                    out_data[i] = data[j];
            }
            out_count = i;

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 
        }

        if (debug) {
            std::cout << "wrote until to char in out array: " << out_count << std::endl;
        }
        is_write = false;
        ret_val = out_count - start_count;
    }

    return(ret_val);
}

// Convert attributes and data of tag element to csv format
// Used when tag is marked with ,c and then basically everything under tag is converted
long write_array_conv(char *out_data, char *data, long &out_count, long pos_tag, long pos_end, long taglen, long debug)
{
    long start_count = out_count;
    long ret_val = 0;
    long i = 0; 
    long j = 0;
    long k = 0; 
    long l = 0;
    long checked = 0;
    char left = '|';
    char right = '|';
    long wstart = 0;
    long wend = 0;
    bool is_write = false;
    bool is_set = false;
    long z = 0;

    if (debug) {
        std::cout << "pos_tag: " << pos_tag << std::endl;
        std::cout << "pos_end:: " << pos_end << std::endl;
        std::cout << "out_count: " << out_count << std::endl;
        std::cout << "taglen: " << taglen << std::endl;
    }

    // Loop all characterss between start and end tags
    while (checked < (pos_end - pos_tag - taglen)) {

        left = '|';
        right = '|';
        // Note that l remember last round
        for (k = 0; l < (pos_end - pos_tag - taglen) && !is_write; k++, l++, is_set = false) {
            if (data[pos_tag + 1 + l] == '"' && data[pos_tag + l] == '=' && left == '|' && right == '|' && !is_set) {
                left = '"';
                wstart = (pos_tag + 2 + l);
                is_set = true;
            }
            if (data[pos_tag + 1 + l] == '>' && left == '|' && right == '|' && !is_set) {
                left = '>';
                wstart = (pos_tag + 2 + l);
                is_set = true;
            }
            if (data[pos_tag + 2 + l] == '"' && left == '"' && right == '|' && !is_set) {
                right = data[pos_tag + 1 + l];
                wend = (pos_tag + 1 + l);
                is_set = true;
                z = wstart;
                while (z <= wend) {
                    if (data[z] != ' ' && data[z] != '\n') {
                        is_write = true;
                    }
                    z++;
                }
                if (!is_write) {
                    left = '|';
                    right = '|';
                    wstart = wend;
                }            
            }
            if (data[pos_tag + 2 + l] == '<' && left == '>' && right == '|' && !is_set) {
                right = data[pos_tag + l];
                wend = (pos_tag + 1 + l);
                is_set = true;
                z = wstart;
                while (z <= wend) {
                    if (data[z] != ' ' && data[z] != '\n') {
                        is_write = true;
                    }
                    z++;
                }
                if (!is_write) {
                    left = '|';
                    right = '|';
                    wstart = wend;
                }            
            }
            if (debug) {
                std::cout << "check char: " << (pos_tag + 1 + l) << std::endl;
                std::cout << "check char: " << data[pos_tag + 1 + l] << std::endl;
                std::cout << "end value: " << (pos_end - pos_tag - taglen) << std::endl;
                std::cout << "current value: " << l << std::endl;
                // In debug limit data of tag to 201 char
                if (l > 200) {
                    break;
                }
            }
        }
        checked = l;

        if (debug) {
            std::cout << "checked: " << checked << std::endl;
            std::cout << "left: " << left << std::endl;
            std::cout << "rigth: " << right << std::endl;
            std::cout << "is_write: " << is_write << std::endl;
            std::cout << "watart: " << wstart << std::endl;
            std::cout << "wend: " << wend << std::endl;
        }

        if (is_write) {

            if (debug) {
                std::cout << "start write to array" << std::endl;
            }
            // Comma separator
            if (out_count > 0) {
                out_data[out_count] = (char) 0x2c;
                out_count++; 
            }

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 

            // Data of tag element
            for (i = out_count, j = wstart; j <= wend; i++, j++) {
                    out_data[i] = data[j];
            }
            out_count = i;

            // Quotation mark
            out_data[out_count] = (char) 0x22;
            out_count++; 
        }

        if (debug) {
            std::cout << "wrote until to char in out array: " << out_count << std::endl;
        }
        is_write = false;
        ret_val = out_count - start_count;
    }

    return(ret_val);
}

// Plain write data of tag element to csv format without any conversion logic
long write_array(char *out_data, char *data, long &out_count, long pos_tag, long pos_end, long taglen)
{
    long start_count = out_count;
    long ret_val = 0;
    int i = 0; 
    int j = 0;
    // Comma separator
    if (out_count > 0) {
        out_data[out_count] = (char) 0x2c;
        out_count++; 
    }

    // Quotation mark
    out_data[out_count] = (char) 0x22;
    out_count++; 

    // Data of tag element
    for (i = out_count, j = 0; j < (pos_end - pos_tag - taglen); i++, j++) {
        out_data[i] = data[pos_tag + 1 + j];
    }
    out_count = out_count + j;

    // Quotation mark
    out_data[out_count] = (char) 0x22;
    out_count++; 

    ret_val = out_count - start_count;
    return(ret_val);
}

// Finds tag from input data in array
long find_tag(char *tag_search, char *data, long start_count, long debug)
{
    long lenstr_tag = std::strlen(tag_search);
    long cmpcount = 0;
    long datacount = start_count;
    int same_count = 0;
    if (lenstr_tag > 50 || lenstr_tag == 0) {
        return(-1);
    }
    while (data[datacount] != '\0' && tag_search[cmpcount] != '\0' && cmpcount < lenstr_tag) {
        if (debug) {
            std::cout << cmpcount << lenstr_tag << same_count << datacount << std::endl;
        }
        if (data[datacount] == tag_search[cmpcount]) {
            if (debug) {
                std::cout << "same char\n";
            }
            same_count++;
            if (same_count == lenstr_tag) {
                // Tag with attribute tag (attribute next one) add position 1 becouse missing char >
                if (tag_search[0] == '<' && tag_search[lenstr_tag - 1] != '>') {
                    datacount++;
                }
                if (debug) {
                    std::cout << "found tag\n";
                    std::cout << "position: " << datacount << std::endl;
                }
                return(datacount);
            } else {
                cmpcount++;
            }
        } else {
            if (debug) {
                std::cout << data[datacount] << " " << tag_search[cmpcount] << tag_search << " different char\n";
            }
            same_count = 0;
            cmpcount = 0;
        }
        datacount++;
    }
    //std::cout << "search ended\n";
    return(0);
}

// Get next tag input tag data
long get_tag(char *tag_point, char *data, long &type, char &conv, long start_count)
{
    long datacount = start_count;
    long tagcount = 0;
    long lenstr_tag = std::strlen(data);
    if (start_count > 0) {
        datacount++;
    }
    while (data[datacount] != '\0' && datacount < lenstr_tag) {
        if (data[datacount] == ',') {
            tag_point[tagcount] = '\0';
            datacount++;
            type = atoi(&data[datacount]);
            if (data[datacount + 1] == ',') {
                datacount++;
                datacount++;
                conv = data[datacount];    
            }
            return(datacount);
        }
        tag_point[tagcount] = data[datacount];
        tagcount++;
        datacount++;
    }
    return(0);
}

// Input: tag_end pointer for return ending tag. tag_point pointer to starting tag.
// Output: Return value length of ending tag. at tag_end pointer to ending tag
// If starting tag last char is " then tag for attribute
long get_tag_end(char *tag_end, char *tag_point)
{
    long taglen = std::strlen(tag_point);
    if (tag_point[taglen - 1] == '"') {
        tag_end[0] = '"';
        tag_end[1] = '\0';
    } else {    
        for(int i=0; i <= taglen; i++) {
            if (i == 1) {
                tag_end[i] = '/';
            } else if (i > 1) {
                tag_end[i] = tag_point[i-1];
            } else {
                tag_end[i] = tag_point[i]; 
            }
        }
        tag_end[taglen+1] = '\0';
    }
    taglen = std::strlen(tag_end);

    return(taglen);
}

// Read whole tag file into array
long read_tags(char *filename, char *data, long amount) {
    long read_amount = 0;

    dbios obj_tag;
    obj_tag.db_open(filename, db_in);
    read_amount = obj_tag.db_read_by_one(data, amount);
    obj_tag.db_close();
    return(read_amount);
}

// Read chunk of input data to array return object for read more data and close file
long read_data_left_open(dbios &obj_data, char *filename, char *data, long amount) {
    long read_amount = 0;

    obj_data.db_open(filename, db_in);
    read_amount = obj_data.db_read_bulk(data, amount);
    return(read_amount);
}

// Read more chunks from already opened file
long read_data_more(dbios &obj_data, char *data, long amount) {
    long read_amount = 0;

    read_amount = obj_data.db_read_bulk(data, amount);
    return(read_amount);
}

// Closes file used as input for data
long read_data_close(dbios &obj_data) {
    obj_data.db_close();
    return(0);
}

// Write chunk of csv data to output file and return object for write more chunks and close the file
long write_data_left_open(dbios &obj_data, char *filename, char *data, long amount) {
    long write_amount = 0;

    obj_data.db_open(filename, db_out);
    obj_data.db_write(data, amount);
    return(write_amount);
}

// Writes more chunks to output file
long write_data_more(dbios &obj_data, char *data, long amount) {
    long write_amount = 0;

    obj_data.db_write(data, amount);
    return(write_amount);
}

// Closes output file
long write_data_close(dbios &obj_data) {
    obj_data.db_close();
    return(0);
}

static void show_usage(std::string name) {
    std::cout << "Usage: " << name << " <options>\n"
              << "Options:\n"
              << "\t-h, --help\t\t\tShow this help message\n"
              << "\t-t, --tagfile <filename>\tSpecify name of the file for tags\n"
              << "\t-i, --input <filename>\t\tSpecify name of the file for input\n"
              << "\t-o, --output <filename>\t\tSpecify name of the file for output\n"
              << "Additional options without flag (not required):\n"
              << "\tMax row length (default 8192)\n"
              << "\tTag file max length in chars (default 1024)\n"
              << "\tTag max length (default 50)\n"
              << "\tMax columns count (default 20)\n"
              << "\tDegug when 1 (default 0)\n"
              << "\tDebug2 when 1 (default 0)\n"
              << std::endl;
    std::cout << "Example:\n"
              << name 
              << " -t files/tag_file.txt -i files/myfile.txt -o files/myout.txt 5000 1024 50 10 0 0\n"
              << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------------"
              << std::endl;
    std::cout << "Tag file examples:\n"
              << "To read elements present only once or missing\n"
              << "\t<person,0\n"
              << "\tidentifier=\",1\n"
              << "\tidentifier2=\",1\n"
              << "\t<name,1,c\n"
              << "\t<address,1,l\n"
              << "\t<gender>,1\n"
              << "\t<birthdate>,1\n"
              << "\t<phone,1,o\n"
              << "\t<education>,1\n"
              << "To read elements present many times\n"
              << "\t<person,0\n"
              << "\tidentifier=\",1\n"
              << "\tidentifier2=\",1\n"
              << "\t<workhistory>,2\n"
              << "From XML file like\n"
              << "\t<person identifier=\"12345\" identifier2=\"54321\">\n"
              << "\t  <name nickname=\"Jhonny\">\n"
              << "\t    <firstname>John</firstname>\n"
              << "\t    <middlename>Miles</middlename>\n"
              << "\t    <lastname>Smith</lastname>\n"
              << "\t  </name>\n"
              << "\t  <address country=\"USA\">Dallas</address>\n"
              << "\t  <gender>male</gender>\n"
              << "\t  <birthdate>10.11.1964</birthdate>\n"
              << "\t  <phone location=\"home\">555-555</phone>\n"
              << "\t  <education>PhD</education>\n"
              << "\t  <workhistory>trainee at company A</workhistory>\n"
              << "\t  <workhistory>clerk at company B</workhistory>\n"
              << "\t  <workhistory>manager at company C</workhistory>\n"
              << "\t</person>\n"
              << "Then special cases in tag file\n"
              << "Convert with\n"
              << "\t<name,1,c\n"
              << "For\n"
              << "\t<name nickname=\"Jhonny\">\n"
              << "\t  <firstname>John</firstname>\n"
              << "\t  <middlename>Miles</middlename>\n"
              << "\t  <lastname>Smith</lastname>\n"
              << "\t</name>\n"
              << "See content when testing with\n" 
              << "\t<name,1\n"
              << "Get only Smith with\n" 
              << "\t<name,1,l\n"
              << "Get attrbutes and Smith with\n" 
              << "\t<name,1,o\n"
              << "Get attributes and Smith and child elements\n"
              << "\t<name,1,o\n"
              << "\t<firstname>,1\n"
              << "\t<middlename>,1\n"
              << "Also works\n"
              << "\t<name,1,l\n"
              << "\t<nickname=\",1\n"
              << "\t<firstname>,1\n"
              << "\t<middlename>,1\n"
              << std::endl;
}

// Main of tag_data_reader
int main(int argc, char *argv[]) {

    long param1 = 8192L;                // Input: Row max length default 8192
    long param2 = 1024L;                // Input: Tag file max length default 1024
    long param3 = 50L;                  // Input: Tag max length default 50
    long param4 = 20L;                  // Input: Max columns
    long param5 = 0L;                   // Input: Debug when 1 default 0
    long param6 = 0L;                   // Input: Debug2 when 1 default 0

    long p_inp1;                        // Input: From command line or param Row max length default 8192
    long p_inp2;                        // Input: From command line or param Tag file max length default 1024
    long p_inp3;                        // Input: From command line or param Tag max length default 50
    long p_inp4;                        // Input: From command line or param Max columns
    long p_inp5;                        // Input: From command line or param Debug when 1 default 0
    long p_inp6;                        // Input: From command line or param Debug2 when 1 default 0

    switch (argc) {
    case 7:
        p_inp1 = param1;
        p_inp2 = param2;
        p_inp3 = param3;
        p_inp4 = param4;
        p_inp5 = param5;
        p_inp6 = param6;
        break;
    
    case 8:
        p_inp1 = strtol(argv[7], NULL, 10);
        p_inp2 = param2;
        p_inp3 = param3;
        p_inp4 = param4;
        p_inp5 = param5;
        p_inp6 = param6;
        break;

    case 9:
        p_inp1 = strtol(argv[7], NULL, 10);
        p_inp2 = strtol(argv[8], NULL,10);
        p_inp3 = param3;
        p_inp4 = param4;
        p_inp5 = param5;
        p_inp6 = param6;
        break;

    case 10:
        p_inp1 = strtol(argv[7], NULL, 10);
        p_inp2 = strtol(argv[8], NULL, 10);
        p_inp3 = strtol(argv[9], NULL, 10);
        p_inp4 = param4;
        p_inp5 = param5;
        p_inp6 = param6;
        break;

    case 11:
        p_inp1 = strtol(argv[7], NULL, 10);
        p_inp2 = strtol(argv[8], NULL, 10);
        p_inp3 = strtol(argv[9], NULL, 10);
        p_inp4 = strtol(argv[10], NULL, 10);
        p_inp5 = param5;
        p_inp6 = param6;
        break;

    case 12:
        p_inp1 = strtol(argv[7], NULL, 10);
        p_inp2 = strtol(argv[8], NULL, 10);
        p_inp3 = strtol(argv[9], NULL, 10);
        p_inp4 = strtol(argv[10], NULL, 10);
        p_inp5 = strtol(argv[11], NULL, 10);
        p_inp6 = param6;
        break;

    case 13:
        p_inp1 = strtol(argv[7], NULL, 10);
        p_inp2 = strtol(argv[8], NULL, 10);
        p_inp3 = strtol(argv[9], NULL, 10);
        p_inp4 = strtol(argv[10], NULL, 10);
        p_inp5 = strtol(argv[11], NULL, 10);
        p_inp6 = strtol(argv[12], NULL, 10);
        break;

    defaul:
        break;
    }

    char tag_file[1025];                    // File for tags 
    char data_tag[p_inp2 + 1];              // Array for tags
    long chunk_tag = p_inp2;                // Bytes tried to read
    long read_amount_tag = 0;               // Bytes read from tagfile
    char tag_word[p_inp3 + 1];              // Array for tag
    char tag_end[p_inp3 + 2];               // Array for tag end
    long tag_type = 0;                      // Type of tag
    long tag_pos = 0;                       // Tag last char position in tag array
    long taglen = 0;                        // Length of tag
    char data_file[1025];                   // File for data 
    char data[2 * p_inp1 + 1];              // Array for data
    char data2[2 * p_inp1 + 1];             // Array 2 for data
    long chunk_data = 2 * p_inp1;           // Bytes tried to read
    long read_amount_data = 0;              // Bytes read from datafile
    long pos_tag = 0;                       // Tag last char position in data array
    long pos_end = 0;                       // Tag end last char position in data array
    long pos_tag_row = 0;                   // Tag for row last char position in data array
    long pos_end_row = 0;                   // Tag end for row last char position in data array
    long pos_tag_prev = 0;                  // Tag previous position when new tag not found
    char out_file[1025];                    // File for output 
    char out_data[2 * p_inp1 + 1];          // Array for out_data
    long chunk_out = 2 * p_inp1;            // Bytes tried to write
    long write_amount_out = 0;              // Bytes read from datafile
    long out_count = 0;                     // Pointer in out array
    long fclose = 0;                        // Return from file close
    bool first_chunk = true;                // To pass first eof check
    long debug = p_inp5;                    // Debug when 1
    long col_count = 0;                     // Count columns
    long col_max = p_inp4;                  // Max columns
    long pos_tag_2 = 0;                     // Tag type 2 last position
    long pos_end_2 = 0;                     // Tag type 2 end tag position
    long pos_tag_2_test = 0;                // tag type 2 testing if tags still in row
    bool is_tag_2_left = false;             // Type 2 tag still in row
    bool is_pos_tag_2_found = false;        // Tag 2 found then can write it
    long data_left = 0;                     // Data left from previous read
    long row_start = 0;                     // Start position of row
    long row_num = 0;                       // Current row number
    long prev_row_num = 0;                  // Remain last row writen
    long added_array = 0;                   // Numver og char added into 
    char conv = 'x';                        // Convert child when c
    long last_row = 0;                      // Last writen row
    long debug2 = p_inp6;                   // More debug

    if (argc < 7) {
        show_usage(argv[0]);
        return 1;
    }
    std::string tagfile;
    std::string inputfile;
    std::string outputfile;
    std::string arg;
    for (int ij = 0; ij < 6; ++ij) {
        arg = argv[ij];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if ((arg == "-t") || (arg == "--tagfile")) {
            if (ij + 1 < argc) { // Make sure we aren't at the end of argv!
                ij++;
                tagfile = argv[ij];
                if (tagfile.length() > 1024) {
                    std::cout << "In --tagfile option filename too long (max. 1024 char)." << std::endl;    
                }
            } else { // Uh-oh, there was no argument to the destination option.
                  std::cout << "--tagfile option requires one argument." << std::endl;
                return 1;
            }  
        } else if ((arg == "-i") || (arg == "--input")) {
            if (ij + 1 < argc) { // Make sure we aren't at the end of argv!
                ij++;
                inputfile = argv[ij];
                if (inputfile.length() > 1024) {
                    std::cout << "In --input option filename too long (max. 1024 char)." << std::endl;    
                }
            } else { // Uh-oh, there was no argument to the destination option.
                  std::cout << "--input option requires one argument." << std::endl;
                return 1;
            }  
        } else if ((arg == "-o") || (arg == "--output")) {
            if (ij + 1 < argc) { // Make sure we aren't at the end of argv!
                ij++;
                outputfile = argv[ij];
                if (outputfile.length() > 1024) {
                    std::cout << "In --output option filename too long (max. 1024 char)." << std::endl;    
                }
            } else { // Uh-oh, there was no argument to the destination option.
                  std::cout << "--output option requires one argument." << std::endl;
                return 1;
            }  
        }
    }

    strcpy(tag_file, tagfile.c_str());
    strcpy(data_file, inputfile.c_str());
    strcpy(out_file, outputfile.c_str());

    // Object for data file
    dbios obj_data;

    // Object for out file
    dbios obj_out_data;

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Input parameters: " << std::endl;
    std::cout << "tagfile: " << tag_file << std::endl;
    std::cout << "input: " << data_file << std::endl;
    std::cout << "output: " << out_file << std::endl;
    std::cout << "Estimated max row length: " << p_inp1 << std::endl;
    std::cout << "tag_file.txt max length: " << p_inp2 << std::endl;
    std::cout << "Max length of tag: " << p_inp3 << std::endl;
    std::cout << "Max columns: " << p_inp4 << std::endl;
    std::cout << "Debug when 1: " << p_inp5 << std::endl;
    std::cout << "Debug2 when 1: " << p_inp6 << std::endl;
    std::cout << "--------------------------------" << std::endl;

    // Read all tags to memory
    read_amount_tag = read_tags(tag_file, data_tag, chunk_tag);
    if (debug) {
        std::cout << "tag file read bytes: " << read_amount_tag << std::endl;
    }

    // Read first chunk from data file
    read_amount_data = read_data_left_open(obj_data, data_file, data, chunk_data);
    if (debug) {
        std::cout << "read data amount: " << read_amount_data << std::endl;
    }

    // At first read both are same
    data_left = read_amount_data;

    // Rows read and writen in loop to the end of input file
    while (read_amount_data + data_left > 0) {

        // Start of the new row and take the first tag in to use
        if (tag_pos == 0) {

            // Take row number for reference
            row_num++;

            // Init for each row. Tag type 2 is writen to many rows
            is_pos_tag_2_found = false;

            // Get tag start from begin of array
            tag_pos = get_tag(tag_word, data_tag, tag_type, conv, 0);
            if (debug) {
                std::cout << "tag got: " << tag_word << " position: " << tag_pos << " tag type: " << tag_type << std::endl;
                std::cout << "row number: " << row_num << std::endl;
            }

            // Get tag end
            taglen = get_tag_end(tag_end, tag_word);
            if (debug) {
                std::cout << "tag end got: " << tag_end << " len: " << taglen << std::endl;
            }

            // Find tag in data. Start of row data 
            pos_tag_row = find_tag(tag_word, data, pos_tag, debug2);
            if (debug) {
                std::cout << "found: in position " << pos_tag_row << std::endl;
            }
            // Take also absolut start of row
            row_start = (pos_tag_row - strlen(tag_word));

            // Find tag end
            pos_end_row = find_tag(tag_end, data, pos_tag, debug2);
            if (debug) {
                std::cout << "found: in position " << pos_end_row << std::endl;
            }

            // Row end not found in array. Here detect that array is too short
            if (pos_end_row == 0 && pos_tag_row > 0) {
                std::cout << "row longer than input param1: " << p_inp1 << std::endl;
                std::cout << "pos_tag_row: " << pos_tag_row << " pos_end_row: " << pos_end_row << std::endl;
                std::cout << "row number: " << row_num << std::endl;
                return(-1);
            }

            // No row tag anymore in data. Found end of data in array
            // Normally run ends here when no more data read from file and
            // also array is read to end
            if (pos_end_row == 0 && pos_tag_row == 0) {
                std::cout << "no more data found." << std::endl;
                std::cout << "row number: " << last_row << std::endl;
                return(0);
            }
        }

        // Get next tag in array
        tag_pos = get_tag(tag_word, data_tag, tag_type, conv, (tag_pos + 1));
        if (debug) {
            std::cout << "tag got: " << tag_word << " position: " << tag_pos << " tag type: " << 
            tag_type << " conv: " << conv << std::endl;
        }

        // Get tag end
        taglen = get_tag_end(tag_end, tag_word);
        if (debug) {
            std::cout << "tag end got: " << tag_end << " len: " << taglen << std::endl;
        }

        // Do not find row tag again
        //pos_tag = pos_tag_row;

        // Row character count to 0 when start new row
        out_count = 0;

        // Column counter init to count only one row
        col_count = 0;

        // Row transformation. While all tags are read from array
        while (tag_pos > 0) {

            // Break if exceed column max
            // Can experiment giving differen input argument
            col_count++;
            if (col_count > col_max) {
                std::cout << "more columns than max columns: " << col_max << " count: " << col_count << std::endl;
                std::cout << "row number: " << row_num << std::endl;
                break;
            }
            
            // Tag tyoe 2 very different because one tag goes to many rows
            if (tag_type != 2) {
                // Find tag in data
                pos_tag_prev = pos_tag;
                pos_tag = find_tag(tag_word, data, pos_tag, debug2);
                if (debug) {
                    std::cout << "tag found in data in position: " << pos_tag << std::endl;
                }

                // Find tag end
                // Attribute tag end is only 1 char then 1 need to add start position
                pos_end = find_tag(tag_end, data, (pos_tag + 1), debug2);
                if (debug) {
                    std::cout << "tag end found in data in position: " << pos_end << std::endl;
                }

                // If tag is not in data need return previous position
                // This skip missing tag in input data
                if (pos_tag == 0x0000) {
                    pos_tag = pos_tag_prev;
                    pos_end = pos_tag_prev + 1;
                } 
                if (debug) {
                    std::cout << "tag not found in data. pos_tag: " << pos_tag << " pos_end: " << pos_end << std::endl;
                }
            } else {
                // Find tag type 2 in data
                pos_tag = find_tag(tag_word, data, pos_end_2, debug2);
                pos_tag_2 = pos_tag;
                if (debug) {
                    std::cout << "tag_2 found in data in position: " << pos_tag_2 << std::endl;
                }

                // Find tag end
                // Attribute tag end is only 1 char then 1 need to add start position
                pos_end = find_tag(tag_end, data, (pos_end_2 + 1), debug2);
                pos_end_2 = pos_end;
                if (debug) {
                    std::cout << "tag_2 end found in data in position: " << pos_end_2 << std::endl;
                }
                // There are still same tag in the same row
                if (pos_tag_2 < pos_end_2 && pos_tag_2 > 0) {
                    is_pos_tag_2_found = true;
                }
            }

            if (conv == 'x') {
                // Plain write data to array. Used for simple tag 
                added_array = write_array(out_data, data, out_count, pos_tag, pos_end, taglen);
                if (debug) {
                    std::cout << "added to array: " << added_array << std::endl;
                    std::cout << "data amount writen into output array: " << out_count << std::endl;
                    std::cout << "tag_type: " << tag_type << std::endl;
                    std::cout << "row number: " << row_num << std::endl;
                }
            }

            if (conv == 'c') {
                // Convert data when tag include attributes and element data
                added_array = write_array_conv(out_data, data, out_count, pos_tag, pos_end, taglen, debug2);
                if (debug) {
                    std::cout << "added to array: " << added_array << std::endl;
                    std::cout << "data amount writen into output array: " << out_count << std::endl;
                    std::cout << "tag_type: " << tag_type << std::endl;
                    std::cout << "row number: " << row_num << std::endl;
                }
            }

            if (conv == 'o') {
                // Convert data when tag include attributes and element data. Child element are skipped
                added_array = write_array_outer(out_data, data, out_count, pos_tag, pos_end, taglen, debug2);
                if (debug) {
                    std::cout << "added to array: " << added_array << std::endl;
                    std::cout << "data amount writen into output array: " << out_count << std::endl;
                    std::cout << "tag_type: " << tag_type << std::endl;
                    std::cout << "row number: " << row_num << std::endl;
                }
            }

            if (conv == 'l') {
                // Convert data when tag element data
                added_array = write_array_last(out_data, data, out_count, pos_tag, pos_end, taglen, debug2);
                if (debug) {
                    std::cout << "added to array: " << added_array << std::endl;
                    std::cout << "data amount writen into output array: " << out_count << std::endl;
                    std::cout << "tag_type: " << tag_type << std::endl;
                    std::cout << "row number: " << row_num << std::endl;
                }
            }

            // Set default
            conv = 'x';

            // For tag type 1 find new tag. For tag_type 2 should not try find new tag.
            if (tag_type != 2) {
                // Get tag
                tag_pos = get_tag(tag_word, data_tag, tag_type, conv, (tag_pos + 1));
                if (debug) {
                    std::cout << "tag got: " << tag_word << " position: " << tag_pos << " tag type: " << 
                    tag_type << " conv: " << conv << std::endl;
                }

                // Get tag end
                taglen = get_tag_end(tag_end, tag_word);
                if (debug) {
                    std::cout << "tag end got: " << tag_end << " len: " << taglen << std::endl;
                }
            } else {
                pos_tag_2_test = find_tag(tag_word, data, pos_end_2, debug2);
                // Test if still tag type 2 in same row
                if (pos_tag_2_test < pos_end_row && pos_tag_2_test > 0) {
                    is_tag_2_left = true;
                } else {
                    is_tag_2_left = false;
                }
                if (debug) {
                    std::cout << "pos_tag_2_test: " << pos_tag_2_test << " tag: " << tag_word << std::endl;
                    std::cout << "row number: " << row_num << std::endl;
                }
                break;
            }
        }

        // Write data from output array to output file. One write per row
        if (first_chunk) {
            // Write row to output file
            if ((tag_type != 2 || is_pos_tag_2_found) && row_num > prev_row_num) {
                out_data[out_count] = '\n';
                write_amount_out = write_data_left_open(obj_out_data, out_file, out_data, (out_count + 1));
                prev_row_num = row_num;
                if (debug) {
                    std::cout << "first data amount writen into file: " << out_count + 1 << std::endl;
                    std::cout << "row number: " << row_num << std::endl;
                }
                // Next time only eof check
                first_chunk = false;
            }     
        } else {
            if ((tag_type != 2 || is_pos_tag_2_found) && row_num > prev_row_num) {
                // Write row to output file
                out_data[out_count] = '\n';
                write_amount_out = write_data_more(obj_out_data, out_data, (out_count + 1));
                prev_row_num = row_num;
                if (debug) {
                    std::cout << "data amount more writen into file: " << out_count + 1 << std::endl;
                    std::cout << "row number: " << row_num << std::endl;
                }
            }
        }

        // Remember last writen row
        last_row = row_num;

        // Read more data from file. Before move left data to begin of array and append with new data
        // Read if seems that data will run out and not tag type 2 left in row
        if ((chunk_data - pos_end_row) < pos_end_row && !is_tag_2_left) {
            // Read more data
            strcpy(data2, &data[pos_end_row + 1]);
            strcpy(data, data2);
            data_left = strlen(data);
            if (debug) {
                std::cout << "previous read amount: " << read_amount_data << std::endl;
                std::cout << "data left in array: " << data_left << std::endl;
            }
            // Note that case with many rows pos_end_row point end of last row
            read_amount_data = read_data_more(obj_data, &data[chunk_data - pos_end_row - 1], (pos_end_row + 1));
            if (debug) {
                std::cout << "data amount read more from file: " << read_amount_data << std::endl;
                std::cout << "row end: " << pos_end_row << std::endl;
                std::cout << "tried to read amount: " << pos_end_row + 1 << std::endl;
                std::cout << "row number: " << row_num << std::endl;
            }
            // New data read into array then start from begin of the array
            pos_tag_row = 0;
            pos_end_row = 0;
            pos_tag = 0;
            pos_end = 0;
            pos_tag_2 = 0;
            pos_end_2 = 0;
            row_start = 0;
        // For tag type 2 is writen from one row to many rows
        } else if (is_tag_2_left) {
            pos_tag = row_start;
        // It seems that thereis still enoug data in array left
        } else {
            // Not read new data then continue read array for the next row
            pos_tag = pos_end_row;
        }
        if (debug) {
            std::cout << "pos_tag value at the row loop end: " << pos_tag << std::endl;
            std::cout << "pos_end value at the row loop end: " << pos_end << std::endl;
            std::cout << "pos_tag_row value at the row loop end: " << pos_tag_row << std::endl;
            std::cout << "pos_end_row value at the row loop end: " << pos_end_row << std::endl;
            std::cout << "pos_end_2 value at the row loop end: " << pos_end_2 << std::endl;
            std::cout << "row_start value at the row loop end: " << row_start << std::endl;
            std::cout << "row number: " << row_num << std::endl;
        }
    }

    // The end close files
    fclose = write_data_close(obj_out_data);
    fclose = read_data_close(obj_data);
    if (debug) {
        std::cout << "data files closed" << std::endl;
    }

    return 0;
}
