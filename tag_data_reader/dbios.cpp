//-----------------------------------------------------------------------
// VERSION 1.0.0
// FILE: dbios.cpp
// OBJECT: dbios
// DESCRIPTION: This file includes methods of dbios class. Class is 
// documented in dbios.h file. These functions are used to handling 
// files. Open files, write and read. Reason for this is possibility 
// add encryption.
//
// CREATED: 23.5.2000
// MODIFIED:
// MODIFIED BY:
// HISTORY:
// 2021-05-27 HarriK Modification for g++
//-----------------------------------------------------------------------
// Copyright 2000 Harri Kalikka
//-----------------------------------------------------------------------

#include "dbios.h"

// Only usual cases. Need handling with exeptions and too long
// or bad filename.
void dbios::db_open(char *name, int flag)
{
	strncpy(Fname, name, 1025);

	switch(flag){

	case db_in:
		dbfile.open(name, std::ios::in);
		break;

	case db_out:
		dbfile.open(name, std::ios::out);
		break;

	case db_binary | db_in | db_out:
		dbfile.open(name, std::ios::binary | std::ios::in | std::ios::out);
		break;

	defaulf:
		break;
	}
}

// This is just like that.
void dbios::db_close()
{
	dbfile.close();
}

// write function have to have possibility to 
// encrypt data. There is needed parameter and this function
// can be used also if later dbfiles with plain text is needed
// to encrypt. 
void dbios::db_write(char *data, long amount)
{
	dbfile.write(data, amount);
}

long dbios::db_read_by_one(char *data, long amount)
{
	long pointer = 0;
	char rdata[1];
	while(!this->db_eof() && pointer < amount) { 
		dbfile.read(rdata, 1);
		data[pointer] = rdata[0];
		pointer++;
	}
	if (this->db_eof()) {
		pointer--;
	}
	data[pointer] = '\0';
	return (pointer);
}

long dbios::db_read_bulk(char *data, long amount)
{
	long pointer = 0;
	dbfile.read(data, amount);
	pointer = this->db_gcount();
	data[pointer] = '\0';
	return (pointer);
}

int dbios::db_eof()
{
	return(dbfile.eof());
}

long dbios::db_gcount()
{
	return(dbfile.gcount());
}
