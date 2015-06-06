#include <string>
#include <iostream>

#include "dbcomm/DbComm.h"
#include "exception/ThrowableException.h"

using namespace COMMON::DBCOMM;
using namespace COMMON::EXCEPTION;

DbLocation dbLocation1;
DbLocation dbLocation2;
DbLocation dbLocation3;

void ExecuteTest();

void TruncateTest();

void InsertTest();

void SelectTest();

void DeleteTest();

void UpdateTest();

void EscapeString();

void GetPriKeyTest();

void BatchInsertOkTest();

void BatchInsertFailureTest();

void BatchReplaceTest();

void BatchInsertIgnoreTest();

int main()
{   
    dbLocation1.SetDbId("CUMGMDB");
    dbLocation1.SetUser("ppap_cus");
    dbLocation1.SetPassword("abc-qaz");
    
    dbLocation2.SetDbId("CUSLTDB");
    dbLocation2.SetUser("ppap_cus");
    dbLocation2.SetPassword("abc-qaz");
    
    dbLocation3.SetDbId("CUSLTDB");
    dbLocation3.SetUser("ppap_cus");
    dbLocation3.SetPassword("abc-qaz");

    cout << "==================== ExecuteTest() ====================" << endl;
    ExecuteTest();
    cout << "--------------------------------------------------------" << endl;
    
    cout << "==================== TruncateTest() ====================" << endl;
    TruncateTest();
    cout << "--------------------------------------------------------" << endl;

    cout << "===================== InsertTest() =====================" << endl;
    InsertTest();
    cout << "--------------------------------------------------------" << endl;

    cout << "===================== SelectTest() =====================" << endl;
    SelectTest();
    cout << "--------------------------------------------------------" << endl;

    cout << "===================== UpdateTest() =====================" << endl;
    UpdateTest();
    cout << "--------------------------------------------------------" << endl;

    cout << "===================== DeleteTest() =====================" << endl;
    DeleteTest();
    cout << "--------------------------------------------------------" << endl;
    
    cout << "===================== EscapeString() =====================" << endl;
    EscapeString();
    cout << "--------------------------------------------------------" << endl;
    
    cout << "===================== GetPriKeyTest() =====================" << endl;
    GetPriKeyTest();
    cout << "--------------------------------------------------------" << endl;
    
    cout << "===================== BatchInsertOkTest() =====================" << endl;
    TruncateTest();
    BatchInsertOkTest();
    cout << "--------------------------------------------------------" << endl;
    
    cout << "===================== BatchInsertFailureTest() =====================" << endl;
    BatchInsertFailureTest();
    cout << "--------------------------------------------------------" << endl;
    
    cout << "===================== BatchReplaceTest() =====================" << endl;
    BatchReplaceTest();
    cout << "--------------------------------------------------------" << endl;
    
    cout << "===================== BatchInsertIgnoreTest() =====================" << endl;
    TruncateTest();
    BatchInsertIgnoreTest();
    BatchInsertIgnoreTest();
    cout << "--------------------------------------------------------" << endl;
}

void GetPriKeyTest()
{
    cout << "Test to get the primary keys of TEST.tbl_test\n";
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);
    
        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
                    
        db2Tasks->Connect();
        
        DbQueryAction* query_action = db2Tasks->GetPriKeys();
        
        DbGetPriKeysFilter filter("TEST.tbl_test");
        
        query_action->Do(&filter);
        
        DbQueryRslt* query_rslt = (DbQueryRslt*)query_action->GetRslt();

        Row rslt;
		bool success = false;
        for (int i = 0; i < dbLocations_array.size(); i++)
        {
			cout << "Here are the primary keys of TABLE TEST.tbl_test in " << dbLocations_array[i].GetDbId() << endl;
            while ((char**)(rslt = query_rslt->Fetch(&(dbLocations_array[i]), success)) != NULL) 
            {
				// the 0 is the only available index when you are getting the primary key
                cout << rslt[0] << endl;
            }    
			cout << endl;
        }
        
        query_action->EndAction();
        db2Tasks->Disconnect();    
    }
    catch(ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
}

void BatchReplaceTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        DbExecuteAction* replace_action = db2Tasks->BatchReplace(5000, 2);

        // first
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("Peter");
            column_value["col2"] = Value("good");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);    
        }
        
        // second
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("John");
            column_value["col2"] = Value("better");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);  
        }
        
        // third
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("Marry");
            column_value["col2"] = Value("best");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);    
        }
        
        replace_action->EndAction();

        DbInsertRslt* insert_rslt = (DbInsertRslt*)replace_action->GetRslt();

        bool success = true;
        map<DbLocation, long long> affected_rows = insert_rslt->GetAffectedRows(success);
        map<DbLocation, long long>::iterator it = affected_rows.begin();
        for (; it != affected_rows.end(); it++)
        {
            cout << "INSERT INTO " << it->first.ToString() << " [" << it->second << "]" << endl;
        }

        db2Tasks->Disconnect();
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }       
}

void BatchInsertFailureTest()
{
    TruncateTest();
    BatchInsertOkTest();
    BatchInsertOkTest();
}

void BatchInsertOkTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        DbExecuteAction* insert_action = db2Tasks->BatchInsert(5000, 2);

        // first
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("Peter");
            column_value["col2"] = Value("good");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);    
        }
        
        // second
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("John");
            column_value["col2"] = Value("better");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);  
        }
        
        // third
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("Marry");
            column_value["col2"] = Value("best");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);    
        }
        
        insert_action->EndAction();

        DbInsertRslt* insert_rslt = (DbInsertRslt*)insert_action->GetRslt();

        bool success = true;
        map<DbLocation, long long> affected_rows = insert_rslt->GetAffectedRows(success);
        map<DbLocation, long long>::iterator it = affected_rows.begin();
        for (; it != affected_rows.end(); it++)
        {
            cout << "INSERT INTO " << it->first.ToString() << " [" << it->second << "]" << endl;
        }

        db2Tasks->Disconnect();
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }    
}

void BatchInsertIgnoreTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        DbExecuteAction* insert_action = db2Tasks->BatchInsertIgnore(5000, 2);

        // first
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("Peter");
            column_value["col2"] = Value("good");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);    
        }
        
        // second
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("John");
            column_value["col2"] = Value("better");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);  
        }
        
        // third
        {
            map<string, Value> column_value;

            column_value["col1"] = Value("Marry");
            column_value["col2"] = Value("best");
            
            BatchFilter replaceFilter("TEST.tbl_test", column_value);
            replace_action->Do(&replaceFilter);    
        }
        
        insert_action->EndAction();

        DbInsertRslt* insert_rslt = (DbInsertRslt*)insert_action->GetRslt();

        bool success = true;
        map<DbLocation, long long> affected_rows = insert_rslt->GetAffectedRows(success);
        map<DbLocation, long long>::iterator it = affected_rows.begin();
        for (; it != affected_rows.end(); it++)
        {
            cout << "INSERT INTO " << it->first.ToString() << " [" << it->second << "]" << endl;
        }

        db2Tasks->Disconnect();
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }    
}

void EscapeString()
{
    // a string with " ' ", "\0", "\n", "\r"
    string str("abc\0efg\nhijkl'123'\ra", 20);
    
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        EscapeStringAction* escape_string_action 
            = (EscapeStringAction*)db2Tasks->EncodeEscapedString();
        
        EncodeEscapedSrtingFilter filter(str.data(), str.length());

        escape_string_action->Do(&filter);

        string escaped_string = escape_string_action->GetEscapedString();

        cout << "string after escaped : " << escaped_string << endl;
        
        escape_string_action->EndAction();

        db2Tasks->Disconnect();
    }
    catch(ThrowableException& e)
    {
        cout<<e.What(true)<<endl;
    }
}

void ExecuteTest()
{
    const string create_tbl_statement = "create table TEST.tbl_test(col1 char(10) not null primary key,col2 varchar(20),col3 decimal(8,2),col4 double,col5 integer,col6 timestamp)";
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        DbExecuteAction* action = db2Tasks->Execute();
        ExecuteFilter filter1(create_tbl_statement);
        action->Do(&filter1);
        action->EndAction();

        db2Tasks->Disconnect();
        
        printf("ExecuteTest() run ok\n");
    }
    catch(ThrowableException& e)
    {
        // if failure, drop the existing table first and then create one
        try
        {
            vector<DbLocation> dbLocations_array;
            dbLocations_array.push_back(dbLocation1);
    
            tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
            db2Tasks->Connect();
    
            DbExecuteAction* action = db2Tasks->Execute();
            ExecuteFilter filter1("drop table TEST.tbl_test");
            ExecuteFilter filter2(create_tbl_statement);
            action->Do(&filter1);
            action->Do(&filter2);
            action->EndAction();
    
            db2Tasks->Disconnect();
            
            printf("ExecuteTest() run ok\n");
        }
        catch(ThrowableException& e)
        {
            cout<<e.What()<<endl;
            exit(0);
        }
    }
}

void TruncateTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);
    
        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
    
      	db2Tasks->Connect();
      	
      	bool success = false;
      	
        DbExecuteAction* action = db2Tasks->Truncate();
		
        // truncate table use the DB2 ways
        TruncateFilter filter1("alter table TEST.tbl_test activate not logged initially with empty table");
    
        action->Do(&filter1);
        
        action->EndAction();
        
        db2Tasks->Disconnect();	
        
        printf("truncate table TEST.tbl_test ok!\n");
    }
    catch(ThrowableException& e)
    {
        cout<<e.What()<<endl;
    }
}

void InsertTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        
      	db2Tasks->Connect();
        DbExecuteAction* action = db2Tasks->Insert(5000);
        
        InsertFilter filter1("insert into TEST.tbl_test(col1,col5) values('Peter',0)");
        InsertFilter filter2("insert into TEST.tbl_test(col1,col5) values('John',200)");
        InsertFilter filter3("insert into TEST.tbl_test values('Marry','describe it',998.12,3.14,502,'2015-01-28-10.28.50.932234')");
        InsertFilter filter4("insert into TEST.tbl_test values('Richard','describe it for',999.89,3.141592654,402,'2015-01-27-10.28.50.865423')");

        action->Do(&filter1);
        action->Do(&filter2);
        action->Do(&filter3);
        action->Do(&filter4);
        action->EndAction();
        
        db2Tasks->Disconnect();	
        printf("insert ok\n");
    }
    catch(ThrowableException& e)
    {
       cout<<e.What(true)<<endl;
    }
}

void SelectTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        DbQueryAction* query_action = db2Tasks->Select();

        QueryFilter selectFilter("select col1, col2, col3, col4, col5, col6 from TEST.tbl_test");

        query_action->Do(&selectFilter);

        DbQueryRslt* query_rslt = (DbQueryRslt*)query_action->GetRslt();

        Row rslt;
        int count = 0;
        bool success = false;

        while ((char**)(rslt = query_rslt->Fetch(success)) != NULL) 
        {
            unsigned long * lengths = query_rslt->GetCurrentRowColumnsLength(success);

            printf("column 1: length[%4ld] test[%s]\n",lengths[rslt.GetIndexByColName("col1")], rslt["col1"]);
            printf("column 2: length[%4ld] test[%s]\n",lengths[rslt.GetIndexByColName("col2")], rslt["col2"]);
            printf("column 3: length[%4ld] test[%s]\n",lengths[rslt.GetIndexByColName("col3")], rslt["col3"]);
            printf("column 4: length[%4ld] test[%s]\n",lengths[rslt.GetIndexByColName("col4")], rslt["col4"]);
            printf("column 5: length[%4ld] test[%s]\n",lengths[rslt.GetIndexByColName("col5")], rslt["col5"]);
            printf("column 6: length[%4ld] test[%s]\n",lengths[rslt.GetIndexByColName("col6")], rslt["col6"]);
            printf("\n");
        }

        query_action->EndAction();

        db2Tasks->Disconnect();

        cout << "select ok" << endl;
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;

    }  
}

void DeleteTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        bool success = false;

		// commit every 5 affected rows
        DbExecuteAction* delete_action = db2Tasks->Delete(5);


        DeleteFilter deleteFilter1("delete from TEST.tbl_test where col1='gong tai'");
        DeleteFilter deleteFilter2("delete from TEST.tbl_test where col1='xu you'");
        delete_action->Do(&deleteFilter1, &dbLocation1);
        delete_action->Do(&deleteFilter2, &dbLocation1);            

        delete_action->EndAction();

        DbDeleteRslt* delete_rslt = (DbDeleteRslt*)delete_action->GetRslt();
        map<DbLocation, long long> affected_rows = delete_rslt->GetAffectedRows(success);
        map<DbLocation, long long>::iterator it = affected_rows.begin();
        for (; it != affected_rows.end(); it++)
        {
            cout << "DELETE FROM " << it->first.ToString() << " [" << it->second << "]" << endl;
        }

        db2Tasks->Disconnect();

        cout << "delete ok" << endl;
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
}

void UpdateTest()
{
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> db2Tasks( new DB2DbTasks(dbLocations_array, true) );
        db2Tasks->Connect();

        UpdateFilter updateFilter1("update TEST.tbl_test set col5 = col5 + 8 where col5 < 500");

        bool success = false;

        DbExecuteAction* update_action = db2Tasks->Update(5);

        update_action->Do(&updateFilter1);

        update_action->EndAction();

        DbUpdateRslt* update_rslt = (DbUpdateRslt*)update_action->GetRslt();

        vector<const DbLocation*> db_targets;
        db_targets.push_back(&dbLocation1);

        map<DbLocation, long long>  rslt = update_rslt->GetAffectedRows(db_targets, success);

        map<DbLocation, long long>::iterator it = rslt.begin();
        for (; it != rslt.end(); it++)
        {
            cout << "UPDATE " << it->first.ToString() << " [" << it->second << "]" << endl;
        }

        db2Tasks->Disconnect();            

        cout << "update ok" << endl;
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
}
