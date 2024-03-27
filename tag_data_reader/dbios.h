#ifndef _dbiosh_
#define _dbiosh_

//--------------------------------------------------------------------------
// VERSION: 1.0.0
// FILE: dbios.h
// OBJECT: dbios
// DESCRIPTION: This class is used to isolate file handling. Using files
// via this class programmers can use encrypted files.
// CREATED: 22.5.2000
// MODIFICATION HISTORY:
// ...
// 2021-05-27 Harri Kalikka Modified for g++
//--------------------------------------------------------------------------
// Copyright 2000 Harri Kalikka
//--------------------------------------------------------------------------
// Documentation
//
// Function open is used open file with openmode flags.
// Usage: obj.open("c:\\path\\filename", db_in | db_out);
//
// Function is_open returns non zero if file is open.
//
// Function fail returns non zero if last operation has failed.
//
// Function good returns non zero if ios::fail, ios::badbit or 
// ios::hardfail is on.
//
//
//
//--------------------------------------------------------------------------
//#include <stdarg>  // fortesting1
#include <fstream>
#include <cstring>
#include <iostream>

// Definitions of openmode flags for open function. Similar like fstream.h.
#define db_in 0x01  // Like ios::in
#define db_out 0x02  // Like ios::out
#define db_ate 0x03  // Like ios::ate
#define db_app 0x04  // Like ios::app
#define db_trunc 0x05  // Like ios::trunc
#define db_nocreate 0x06  // Like ios::nocreate
#define db_noreplace 0x07  // Like ios::noreplace
#define db_binary 0x08  // Like ios::binary

// Definitions of flags for seek functions.
#define db_cur 0x01  // Like ios::cur
#define db_beg 0x02  // Like ios::beg
#define db_end 0x03  // Like ios::end

//
//typedef enum flg_tag { db_in, db_out } open_flag;  // fortesting1

//
typedef char Filename; 

// 
class dbios
{
	Filename Fname[1025];
	std::fstream dbfile;
public:
	dbios() { };
	~dbios() { };
	//void open(char *, open_flag, ...);  // fortesting1
	void db_open(char *, int);
	int db_is_open();
	int db_fail();
	int db_good();
	void db_close();
	void db_write(char *, long);
	long db_read_by_one(char *, long);
	long db_read_bulk(char *, long);
	char db_peek();
	int db_eof();
	void db_seekg(long);
	void db_seekg(long, long);
	long db_tellg();
	void db_seekp(long);
	void db_seekp(long, long);
	long db_tellp();
	void db_clear();
	long db_gcount();
};

#endif

