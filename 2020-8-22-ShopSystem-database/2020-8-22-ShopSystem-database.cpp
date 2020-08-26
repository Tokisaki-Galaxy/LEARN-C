// 2020-8-22-LoginSystem-database.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//虚拟购买系统，数据库操作实例

#define _CRT_SECURE_NO_WARNINGS
#define DEBUG

#include <iostream>
#include <winsock2.h>
#include <mysql.h>
#include <time.h>
#include <string>
#include <vector>

#pragma comment(lib,"libmysql.lib")

using namespace std;

MYSQL			mysqlconn;
MYSQL_RES* mysql_res;//读取记录集
MYSQL_FIELD* mysql_field;//字段对象
MYSQL_ROW		mysql_row;

#ifdef DEBUG
string			current_userid = "aaaa";
#else
string			current_userid = "";
#endif // DEBUG

//初始化数据库
int init() {
	cout << "init mysql database" << endl;
	mysql_init(&mysqlconn);
	cout << "init success" << endl;
	return 0;
}

//链接数据库
int connect()
{
	cout << "linking database...." << endl;
	const char* host = "127.0.0.1";
	const int port = 3306;
	const char* username = "root";
	const char* password = "poipoipoi";
	const char* dbname = "shoping";
	if (mysql_real_connect(&mysqlconn, host, username, password, dbname, port, NULL, CLIENT_FOUND_ROWS) != NULL)
	{
		cout << "link database success" << endl;
		mysql_query(&mysqlconn, "set names gbk");   //设置中文编码
		return 0;
	}
	else {
		cout << "link database fail" << endl;
		return -1;
	}
}

//执行SQL语句
int execsql(const char* sta)
{
	//cout << sta << endl;
	if (mysql_query(&mysqlconn, sta) == 0) {
		//cout << "SQL"<<sta<<"语句执行成功" << endl;
		return 0;
	}
	else
	{
		cout << sta << "语句执行失败,错误信息" << mysql_error(&mysqlconn) << endl;
	}
	return -1;
}

string getDateStr();
string getDate();

//生成订单
int genorder(string itemid)
{
	string sqla, sqlb, sqlc, buyqty = "";
	string orderid = "OI" + getDateStr();
	string orderdate = getDate();

	//下面代码是获取物品的数量
	string sql = "select * from item where itemid='" + itemid + "'";

	if (mysql_query(&mysqlconn, sql.c_str()) == 0) {
		mysql_res = mysql_store_result(&mysqlconn);//将记录集返回
		if (mysql_res) {
			//int fieldcount=mysql_num_fields(mysql_res);
			int rowcount = mysql_num_rows(mysql_res);
			//cout<<"字段数量"<<fieldcount<<endl;
			//cout << "记录总数" << rowcount << endl;
			if (rowcount > 0) {
				mysql_row = mysql_fetch_row(mysql_res);//将一条记录
				//cout << "找到了" << mysql_row[0] << " " << mysql_row[1] << " " << mysql_row[2] << endl;
			}
		}
	}
	cout << "请输入要购买的数量" << endl;
	cin >> buyqty;
	if (stoi(buyqty) < 0 || stoi(buyqty) > stoi(mysql_row[3])) {
		cout << "购买数量超过库存或错误" << endl;
		return -1;
	}

	string tmp = to_string(stoi(mysql_row[3]) - stoi(buyqty));
	sqla = "insert into ordermaster(userid,orderid) values('" + current_userid + "','" + orderid + "');";
	sqlb = "insert into orderdetail(orderid,itmeid,buyqty,date) values('" + orderid + "','" + itemid + "','" + buyqty + "','" + getDate() + "');";
	sqlc = "update item set qty=" + tmp + " where itemid='" + itemid + "'";

	if (execsql(sqlb.c_str()) == 0 && execsql(sqla.c_str()) == 0 && execsql(sqlc.c_str()) == 0) {
		cout << "您将付款" << stoi(mysql_row[4]) * stoi(buyqty) << endl;
		return 0;
	}

	return -1;
}

//新建订单，返回商品编号
string neworder()
{
	string sql = "select * from item";
	string ret = "";

	if (mysql_query(&mysqlconn, sql.c_str()) == 0) {
		mysql_res = mysql_store_result(&mysqlconn);//将记录集返回
		if (mysql_res) {
			int fieldcount = mysql_num_fields(mysql_res);
			int rowcount = mysql_num_rows(mysql_res);
			if (rowcount > 0) {
				cout << "==========商品编号==========" << endl;
				for (int i = 0; i < rowcount; i++) {
					mysql_row = mysql_fetch_row(mysql_res);//抓取一条记录
					cout << "商品编号：" << mysql_row[1] << "    商品名称：" << mysql_row[2] << "    商品数量：" << mysql_row[3] << "    商品价格：" << mysql_row[4] << endl;
				}
			basd:
				cout << "请输入商品编号：";
				cin >> ret;
				if (ret == "")  goto basd; else return ret;
			}
		}
	}
}

//用户登录
int login(string userid, string password)
{
	// char * sql="select  *  from  virusdata where province_confirmedcount>5000";
	//Select 语句
	//string test="hello";

	string sql = "select userid,username,password from  user  where userid='" + userid + "'";

	int sql_ret;//记录 SQL语句的返回值
	//if (mysql_res!=NULL)
	//mysql_free_result(mysql_res);

	sql_ret = mysql_query(&mysqlconn, sql.c_str());
	if (sql_ret == 0) {
		mysql_res = mysql_store_result(&mysqlconn);//将记录集返回
		if (mysql_res) {
			//int fieldcount=mysql_num_fields(mysql_res);
			int rowcount = mysql_num_rows(mysql_res);
			//cout<<"字段数量"<<fieldcount<<endl;
			//cout << "记录总数" << rowcount << endl;
			if (rowcount > 0) {
				mysql_row = mysql_fetch_row(mysql_res);//将一条记录

				//cout << "找到了" << mysql_row[0] << " " << mysql_row[1] << " " << mysql_row[2] << endl;
				if (password == mysql_row[2]) {
					cout << "登录成功" << endl;
					current_userid = userid;
					return 0;
				}
				else {
					cout << "登录失败" << endl;
				}
			}
			else {
				cout << "该用户不存在" << endl;
			}
		}
	}
	return -1;
}

//注册用户
int registeruser()
{
	string userid, username, password;
	cout << "请输入用户ID：";
	cin >> userid;
	cout << "请输入用户名称：";
	cin >> username;
	cout << "请输入用户密码：";
	cin >> password;
	system("cls");
	string a = "insert into user(userid, username, password) values('" + userid + "','" + username + "','" + password + "')";
	if (execsql((char*)a.c_str()) == 0) {
		cout << "注册成功" << endl;
		return 0;
	}
	cout << "注册失败" << endl;
	return -1;
}

//删除用户
int deleteuser()
{
	string userid;
	string password;

	cout << "请输入用户ID：";
	cin >> userid;
	cout << "请输入密码：";
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
	return -1;
}

//校验身份
int verify()
{
	string userid;
	string password;

	cout << "请输入用户编号：";
	cin >> userid;
	cout << "请输入密码：";
	cin >> password;
	if (login(userid, password) == 0) {
		return 0;
	}
	return -1;
}

//指定日期，获得数据库中最新+1的流水号
int getMax(string date)
{
	string sql = "select * from datemax where date='" + date + "'";
	if (mysql_query(&mysqlconn, sql.c_str()) == 0) {
		mysql_res = mysql_store_result(&mysqlconn);//将记录集返回
		if (mysql_res) {
			int rowcount = mysql_num_rows(mysql_res);
			if (rowcount > 0) {
				mysql_row = mysql_fetch_row(mysql_res);//抓取一条记录
				sql = "update datemax set value=" + to_string(stoi(mysql_row[2]) + 1) + " where date='" + date + "'";
				execsql(sql.c_str());
				return stoi(mysql_row[2]) + 1;
			}
			else
			{
				sql = "insert into datemax(date,value) values('" + date + "','" + "1" + "');";
				execsql(sql.c_str());
			}
		}
	}
}

//获得当前日期
string getDate()
{
	time_t t = time(0);
	tm* tm1 = localtime(&t);
	string year, month, day, hour, minute, second;

	year = to_string(tm1->tm_year + 1900);
	month = to_string(tm1->tm_mon + 1);
	day = to_string(tm1->tm_mday);
	hour = to_string(tm1->tm_hour);
	minute = to_string(tm1->tm_min);
	second = to_string(tm1->tm_sec);

	return year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second;
}

//获得当前日期的流水号
string getDateStr()
{
	time_t t = time(0);
	tm* tm1 = localtime(&t);
	string year, month, day, hour, minute, second;

	year = to_string(tm1->tm_year + 1900);
	month = to_string(tm1->tm_mon + 1);
	day = to_string(tm1->tm_mday);
	hour = to_string(tm1->tm_hour);
	minute = to_string(tm1->tm_min);
	second = to_string(tm1->tm_sec);

	return (year + month + day + hour + minute + second + to_string(getMax(year + month + day)));
}

//查询订单
int queryorder() {
	string query;
	int uselect = 0, ia = 0;
	vector<string> ves;
	printf_s("(模糊查询)请输入订单号：");
	cin >> query;
	string sql = "select a.orderid,a.userid,b.date from ordermaster a,orderdetail b where a.orderid=b.orderid and a.orderid like '%" + query + "%'";
	if (execsql(sql.c_str()) == 0) {
		mysql_res = mysql_store_result(&mysqlconn);//将记录集返回
		if (mysql_res) {
			int fieldcount = mysql_num_fields(mysql_res);
			int rowcount = mysql_num_rows(mysql_res);
			if (rowcount > 0) {
				cout << "==========订单列表==========" << endl;
				for (int i = 0; i < rowcount; i++) {
					mysql_row = mysql_fetch_row(mysql_res);//抓取一条记录
					ves.push_back(mysql_row[0]);
					cout << i << "\t订单编号：" << mysql_row[0] << "\t\t订单用户：" << mysql_row[1] << "\t订单日期：" << mysql_row[2] << endl;
				}

			basda:
				printf_s("请输入编号：");
				cin >> uselect;
				if (to_string(uselect) == "")  goto basda;

				string sqla = "select * from orderdetail where orderid='" + ves[uselect] + "'";
				if (execsql(sqla.c_str()) == 0) {
					mysql_res = mysql_store_result(&mysqlconn);//将记录集返回
					if (mysql_res) {
						int fieldcount = mysql_num_fields(mysql_res);
						int rowcount = mysql_num_rows(mysql_res);
						if (rowcount > 0) {
							cout << "==========订单明细==========" << endl;
							for (int i = 0; i < rowcount; i++) {
								mysql_row = mysql_fetch_row(mysql_res);//抓取一条记录
								cout << "购买物品：" << mysql_row[1] << "\t\t购买数量：" << mysql_row[2] << "\t订单日期：" << mysql_row[3] << endl;
								return 0;
							}
						}
					}
				}
			}
			else
			{
				printf_s("无此订单信息\n");
			}
		}
	}

	return -1;
}

int main() {
	init();
	connect();

msa:
	int i = 0;
	printf_s("0.删除用户\n1.注册\n2.登录\n3.购物\n4.查询\n5.退出\n请选择：");
	cin >> i;
	switch (i)
	{
	case 0:
		deleteuser();
		break;
	case 1:
		registeruser();
		break;
	case 2:
		verify();
		break;
	case 3:
		(current_userid != "") ?
			genorder(neworder()) :
			printf_s("请先登录!");
		break;
	case 4:
		(current_userid != "") ?
			queryorder() :
			printf_s("请先登录!");
		break;
	case 5:
		return 0;
		break;
	default:
		goto msa;
		break;
	}

	system("pause&cls");
	goto msa;

	mysql_close(&mysqlconn);
	return 0;
}