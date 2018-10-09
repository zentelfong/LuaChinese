--lua默认函数
垃圾回收=collectgarbage
断言=assert
执行文件=dofile
错误=error
获取环境=getfenv
获取元表=getmetatable
数组迭代=ipairs
迭代=pairs
加载=load
加载文件=loadfile
加载字符串=loadstring
模块定义=module
下一个=next
调用=pcall
打印=print
原始相等=rawequal
原始获取=rawget
原始设置=rawset
包含文件=require
选择=select
设置环境=setfenv
设置元表=setmetatable
转为数字=tonumber
转为字符串=tostring
类型=type
拆开=unpack
保护调用=xpcall

--os
系统={
时钟 = os.clock,
时间 = os.time,
日期 = os.date,
运行 = os.execute,
退出 = os.exit,
获取环境 = os.getenv,
}

--match
数学={
绝对值 = math.abs,

}

--table
表={
连接 = table.concat,
插入 = table.insert,
最大索引 = table.maxn,
移除 = table.remove,
排序 = table.sort
}

--string
字符串={
字节=string.byte,
字符=string.char,

}

