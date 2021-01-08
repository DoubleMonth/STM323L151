echo "# STM323L151" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin git@github.com:DoubleMonth/STM323L151.git
git push -u origin main
                
2020-12-08 建立一个Test工程，实现printf功能，和微秒延时。
2020-12-08 可以正常读取SHT31温湿度，并发送到串口显示。
2020-12-11 正常读取PCF8563时钟，正常驱动2.13寸墨水屏。
2020-12-12 墨水屏GUI显示时钟。
2021-01-09 修改为墨水屏V2程序，可以进入STOP模式 ，外部中断唤醒，设置每分钟局部刷新测试程序，和10分钟全部刷新测试程序，按下设置按键后通过串口更新时间。串口发送时间格式为：202101050234037。

				