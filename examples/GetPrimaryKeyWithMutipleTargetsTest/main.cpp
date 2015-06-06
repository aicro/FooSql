#include <vector>
#include <iostream>
#include <tr1/memory>

#include "dbcomm/DbComm.h"
#include "exception/ThrowableException.h"

using namespace std;
using namespace COMMON::DBCOMM;
using namespace COMMON::EXCEPTION;

int main()
{
	DbLocation dbLocation1;
	dbLocation1.SetDbId("TEST_DB1");
    dbLocation1.SetIp("127.0.0.1");
    dbLocation1.SetPort("3306");
    dbLocation1.SetUser("root");
    dbLocation1.SetPassword("123456");

	DbLocation dbLocation2;
    dbLocation2.SetDbId("TEST_DB2");
    dbLocation2.SetIp("127.0.0.1");
    dbLocation2.SetPort("3306");
    dbLocation2.SetUser("root");
    dbLocation2.SetPassword("123456");
    
	DbLocation dbLocation3;
    dbLocation3.SetDbId("TEST_DB3");
    dbLocation3.SetIp("127.0.0.1");
    dbLocation3.SetPort("3306");
    dbLocation3.SetUser("root");
    dbLocation3.SetPassword("123456");
	
    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);
        dbLocations_array.push_back(dbLocation2);
        dbLocations_array.push_back(dbLocation3);
    
        tr1::shared_ptr<IDbTasks> mysqlTasks( new MysqlDbTasks(dbLocations_array, true) );
        mysqlTasks->Connect();
        
        DbQueryAction* query_action = mysqlTasks->GetPriKeys();
        
        DbGetPriKeysFilter filter1("tbl_test");
        DbGetPriKeysFilter filter2("tbl_test");
        DbGetPriKeysFilter filter3("tbl_test");
        map<DbLocation, DbGetPriKeysFilter*> filter;
        filter[dbLocation1] = &filter1;
        filter[dbLocation2] = &filter2;
        filter[dbLocation3] = &filter3;
        
        query_action->Do(filter);
        
        DbQueryRslt* query_rslt = (DbQueryRslt*)query_action->GetRslt();

        Row rslt;
        bool success = false;
        for (int i = 0; i < dbLocations_array.size(); i++)
        {
			cout << "Here are the primary keys of TABLE tbl_test in " << dbLocations_array[i].GetDbId() << endl;
            while ((char**)(rslt = query_rslt->Fetch(&(dbLocations_array[i]), success)) != NULL) 
            {
				// the 0 is the only available index when you are getting the primary key
                cout << rslt[0] << endl;
            }      
        }
        
        query_action->EndAction();
        mysqlTasks->Disconnect();    
    }
    catch(ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
	
	return 0;
}