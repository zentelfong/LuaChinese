## LuaChinese

旨在提供中文lua运行环境，支持中文标识符，中文（或中英文混合）变量名，中文编码支持gb2312及utf8,如果脚本使用utf8编码编译时请在luaconf.h中定义UNICODE宏后进行编译。

### 支持的中文标识符有

| 中文标识符 | 英文标识符 |
| ---------- | ---------- |
| 如果       | if         |
| 那么       | then       |
| 结束       | end        |
| 否则       | else       |
| 否则如果   | elseif     |
| 循环       | while      |
| 局部变量   | local      |
| 返回       | return     |
| 空         | nil        |
| 真         | true       |
| 假         | false      |
| 不         | not        |
| 没有       | not        |
| 没         | not        |
| 执行       | do         |
| 且         | and        |
| 或         | or         |
| 定义函数   | function   |
| 重复       | repeat     |
| 直到       | until      |
| 中断       | break      |

#### lua自带库函数对应如下

全局函数

| 中文函数名 | lua函数名      |
| ---------- | -------------- |
| 垃圾回收   | collectgarbage |
| 断言       | assert         |
| 执行文件   | dofile         |
| 错误       | error          |
| 获取环境   | getfenv        |
| 获取元表   | getmetatable   |
| 数组迭代   | ipairs         |
| 迭代       | pairs          |
| 加载       | load           |
| 加载文件   | loadfile       |
| 加载字符串 | loadstring     |
| 模块定义   | module         |
| 下一个     | next           |
| 调用       | pcall          |
| 打印       | print          |
| 原始相等   | rawequal       |
| 原始获取   | rawget         |
| 原始设置   | rawset         |
| 包含文件   | require        |
| 选择       | select         |
| 设置环境   | setfenv        |
| 设置元表   | setmetatable   |
| 转为数字   | tonumber       |
| 转为字符串 | tostring       |
| 类型       | type           |
| 拆开       | unpack         |
| 保护调用   | xpcall         |

#### 示例

```
局部变量 数字=12346
打印(数字)
```

