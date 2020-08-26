// 2020-8-22-LoginSystem-database.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <winsock2.h>
#include <mysql.h>
#pragma comment(lib,"libmysql.lib")

using namespace std;
MYSQL mysqlconn;

int init()
{
	cout << "start success,init mysql database" << endl;
	mysql_init(&mysqlconn);
	cout << "init success" << endl;
	return 0;
}

int connect()
{
	cout << "linking database...." << endl;
	const char* host = "127.0.0.1";
	const int port = 3306;
	const char* username = "root";
	const char* password = "poipoipoi";
	const char* dbname = "banking";
	if (mysql_real_connect(&mysqlconn, host, username, password, dbname, port, NULL, CLIENT_FOUND_ROWS) != NULL)
	{
		cout << "link database success" << endl;
		mysql_query(&mysqlconn, "set names gbk");   //设置中文编码
		return 0;
	}
	else {
		cout << "link database fail" << endl;
		return 1;
	}
}

int execsql(char* sta)
{
	if (mysql_query(&mysqlconn, sta) == 0) {
		cout << "SQL语句执行成功" << endl;
		return 0;
	}
	else
	{
		cout << "SQL语句执行失败,错误信息" << mysql_error(&mysqlconn) << endl;
	}
	return 1;
}

int login(string userid, string password)
{
	MYSQL_RES* mysql_res;//读取记录集
	//MYSQL_FIELD* mysql_field;//字段对象
	MYSQL_ROW  mysql_row;

	mysql_query(&mysqlconn, "set names gbk");//将编码设置为中文

   // char * sql="select  *  from  virusdata where province_confirmedcount>5000";//Select 语句
	//string test="hello";

	string sql = "select userid,username,password from  user  where userid='" + userid + "'";

	int sql_ret;//记录 SQL语句的返回值
	//if (mysql_res!=NULL)
   //    mysql_free_result(mysql_res);

	sql_ret = mysql_query(&mysqlconn, sql.c_str());
	if (sql_ret == 0) {
		mysql_res = mysql_store_result(&mysqlconn);//将记录集返回
		if (mysql_res) {
			//int fieldcount=mysql_num_fields(mysql_res);
			int rowcount = mysql_num_rows(mysql_res);
			//cout<<"字段数量"<<fieldcount<<endl;
			cout << "记录总数" << rowcount << endl;
			if (rowcount > 0) {
				mysql_row = mysql_fetch_row(mysql_res);//将一条记录

				cout << "找到了" << mysql_row[0] << " "
					<< mysql_row[1] << " " << mysql_row[2] << endl;
				if (password == mysql_row[2]) {
					cout << "登录成功....." << endl;
					return 0;
				}
				else {
					cout << "登录失败..." << endl;
					return 1;
				}
			}
			else {
				cout << "该用户不存在" << endl;
				return 1;
			}
		}
	}
}

int registeruser() {
	string userid, username, password;
	cout << "请输入用户ID" << endl;
	cin >> userid;
	cout << "请输入用户名称" << endl;
	cin >> username;
	cout << "请输入用户密码" << endl;
	cin >> password;
	system("cls");
	string a = "insert into user(userid, username, password) values('" + userid + "','" + username + "','" + password + "')";
	if (execsql((char*)a.c_str()) == 0) {
		cout << "注册成功" << endl;
		return 0;
	}
	cout << "注册失败" << endl;
	return 1;
}

int deleteuser() {
	string userid;
	string password;

	cout << "请输入用户ID" << endl;
	cin >> userid;
	cout << "请输入密码" << endl;
	cin >> password;
	system("cls");
	if (login(userid, password) == 0) {
		cout << "用户身份确认成功，开始删除操作..." << endl;
		string tmp = "delete from user where userid='" + userid + "'";
		if (execsql((char*)tmp.c_str()) == 0) {
			cout << "删除成功" << endl;
			return 0;
		}
		else
		{
			cout << "删除失败" << endl;
		}
	}
	return 1;
}

int verify() {
	string userid;
	string password;

	cout << "请输入用户编号" << endl;
	cin >> userid;
	cout << "请输入密码" << endl;
	cin >> password;
	login(userid, password);
}

int menu() {
msa:
	int i = 0;
	cout << "1.注册" << endl;
	cout << "2.登录" << endl;
	cout << "3.退出" << endl;
	cout << "10.删除用户" << endl;
	cin >> i;
	switch (i)
	{
	case 1:
		registeruser();
		break;
	case 2:
		verify();
		break;
	case 3:
		exit(0);
		break;
	case 10:
		deleteuser();
		break;
	default:
		goto msa;
		break;
	}
	return 0;
}

int main()
{
	init();
	connect();
meas:
	menu();
	system("pause&cls");
	goto meas;
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧:
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件