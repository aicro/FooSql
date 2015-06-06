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

        vector<DeleteFilter> deleteFilter;
        DeleteFilter deleteFilter1("delete from tbl_test where id = 3");
        DeleteFilter deleteFilter2("delete from tbl_test where id = 4 or id = 5");
        DeleteFilter deleteFilter3("delete from tbl_test where id = 8");

        map<DbLocation, DbActionFilter*> loc_filter;
        loc_filter[dbLocation1] = &deleteFilter1;
        loc_filter[dbLocation2] = &deleteFilter2;
        loc_filter[dbLocation3] = &deleteFilter3;

        bool success = false;
        DbDeleteAction* delete_action = (DbDeleteAction*)mysqlTasks->Delete(5000);

        delete_action->Do(loc_filter);
        delete_action->EndAction();

        DbDeleteRslt* delete_rslt = (DbDeleteRslt*)delete_action->GetRslt();

        vector<const DbLocation*> db_targets;
        db_targets.push_back(&dbLocation1);
        db_targets.push_back(&dbLocation2);
        db_targets.push_back(&dbLocation3);

        map<DbLocation, long long>  rslt = delete_rslt->GetAffectedRows(db_targets, success);

        map<DbLocation, long long>::iterator it = rslt.begin();
        for (; it != rslt.end(); it++)
        {
            cout << "DELETE FROM " << it->first.ToString() << " [" << it->second << "]" << endl;
        }

        mysqlTasks->Disconnect();     
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
    catch (...)
    {
        cout << "unknown exception" << std::endl;
    }
	
	return 0;
}