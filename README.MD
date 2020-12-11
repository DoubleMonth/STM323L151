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
				