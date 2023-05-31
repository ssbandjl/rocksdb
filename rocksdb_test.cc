#include<cstdio>
#include<string>
#include<iostream>
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
using namespace rocksdb;

std::string kDbPath = "/tmp/wsf_rocksdb_test";
std::string kDbPathTran = "/tmp/wsf_rocksdb_transation";

void testTransaction(){
     TransactionDB* txn_db;
     Options options;        
     TransactionDBOptions txn_db_options;
     options.create_if_missing = true;


     Status s = TransactionDB::Open(options,txn_db_options,kDbPathTran,&txn_db );
     Transaction* txn = txn_db->BeginTransaction(WriteOptions());
     s = txn->Put("key", "value");
     s = txn->Delete("key2");
     s = txn->Merge("key3", "value");
     s = txn->Commit();
     delete txn;

}

int main()
{
    DB *db;
    Options options;
    options.create_if_missing = true;

    Status s = DB::Open(options, kDbPath, &db);
    assert(s.ok());

    s = db->Put(WriteOptions(), "k1", "v1");
    assert(s.ok());

    std::string v;
    s = db->Get(ReadOptions(), "k1", &v);
    assert(s.ok());
    std::cout << "v = " << v << std::endl;

    rocksdb::WriteBatch batch;
    batch.Delete("k1");
    batch.Put("k2", "v2");
    batch.Put("k3", "v3");
    batch.Put("k4", "v4");
    s = db->Write(rocksdb::WriteOptions(), &batch);
    if (s.ok()) {
	std::string v2;
	s = db->Get(ReadOptions(), "k2", &v2);
	std::cout << "v = " << v2 << std::endl;
	s = db->Get(ReadOptions(), "k1", &v);
	if (!s.ok()) {
	    std::cout << s.ToString() << std::endl;
	}
	// assert(s.ok());
	//std::cout<<"v = "<<v<<std::endl;
    }

    rocksdb::Iterator * it = db->NewIterator(rocksdb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
	std::cout << it->key().ToString() << ":" << it->value().
	    ToString() << std::endl;
    }

    testTransaction();
}

