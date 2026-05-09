
#[tokio::main]
async fn main() {
    // 1. 创建通道
    let (tx, rx) = tokio::sync::oneshot::channel();

    // 2. 在另一个任务中发送数据
    tokio::spawn(async move {
        let msg = "任务完成！";
        // 使用 send() 发送数据，如果接收端已经掉线，send 会返回 Err
        if let Err(_) = tx.send(msg) {
            println!("接收端已关闭，发送失败");
        }
    });

    // 3. 在主任务中等待数据
    match rx.await {
        Ok(v) => println!("收到结果: {}", v),
        Err(e) => println!("发送端在发送前被丢弃了: {:?}", e),
    }
}