import pymysql  # 导入pymysql模块

def auth(username, password):
    conn = pymysql.connect(
        host = "127.0.0.1",         # mysql主机地址
        port = 3306,                # mysql端口
        user = "root",              # mysql远程连接用户名
        password = "123",            # mysql远程连接密码
        database = "dbf",         # mysql使用的数据库名
        charset = "UTF8"            # mysql使用的字符编码,默认为utf8
    )
    # 打印用户信息: 用户名, 密码
    print("userinfo", username, password)
    # 实例化游标对象
    cursor = conn.cursor(pymysql.cursors.DictCursor)
    # sql查询指令
    sql = "SELECT * FROM userinfo WHERE username=%s AND password=%s"
    # res获取影响行数
    res = cursor.execute(sql, [username, password])
    if res:  # 数据库中存在该数据, 返回True
        return True
    else:  # 数据库中不存在该数据, 返回False
        return False
