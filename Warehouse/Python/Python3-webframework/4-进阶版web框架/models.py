import pymysql  # 导入pymysql模块, 需要下载
# pip install pymysql

def main():
    conn = pymysql.connect(
        host = "127.0.0.1",           # mysql主机地址
        port = 3306,                  # mysql端口
        user = "root",                # mysql远程连接用户名
        password = "123",             # mysql远程连接密码
        database = "dbf",             # mysql使用的数据库名
        charset = "UTF8"              # mysql使用的字符编码,默认为utf8
    )
    # 实例化游标对象
    cursor = conn.cursor(pymysql.cursors.DictCursor)
    # 创建表格
    sql1 = """CREATE TABLE userinfo(
            id int PRIMARY KEY auto_increment,
            username char(12) NOT NULL UNIQUE,
            password char(20) NOT NULL
            );
            """
    # 向创建的表格中插入数据
    sql2 =  """INSERT INTO userinfo(username, password) VALUES
            ("a", "1"),
            ("b", "2");
            """
    # 将sql指令提交到缓存
    cursor.execute(sql1)
    cursor.execute(sql2)
    # 提交并执行sql指令
    conn.commit()
    # 关闭游标
    cursor.close()
    # 关闭与数据库的连接
    conn.close()

if __name__ == "__main__":
    main()  