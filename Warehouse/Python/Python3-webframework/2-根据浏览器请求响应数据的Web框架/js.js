// 定义鼠标覆盖事件触发函数
function mOver(obj)
{
    // 文字替换为"谢谢"
    obj.innerHTML="谢谢"
    // 背景颜色更改为红
    obj.style.backgroundColor= "blue";
}
// 定义鼠标非覆盖状态事件触发函数
function mOut(obj)
{   
    // 文字替换为"把鼠标以到上面"
    obj.innerHTML="把鼠标移到上面"
    // 背景颜色更改为绿
    obj.style.backgroundColor= "green";
}