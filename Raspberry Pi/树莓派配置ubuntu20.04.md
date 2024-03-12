# 树莓派配置ubuntu20.04

## 系统配置

### 镜像

ubuntu mate: https://releases.ubuntu-mate.org/20.04/arm64/

正版ubuntu20.04 desktop的镜像没有找到，目前采用的方法是先装server版，再单独装桌面。server版的镜像在树莓派官方提供的烧录软件里就有，下载地址是https://downloads.raspberrypi.org/imager/imager_latest.exe

### 启动报错

**注意：树莓派启动需要先插显示器再插电源！！！否则显示器无画面**

如果启动的时候报这个错，说明树莓派的固件需要更新，去这个网站下载最新固件https://github.com/raspberrypi/rpi-firmware，看显示什么固件出问题就换对应的就行，即如果提示`start4.elf`不可用，则将系统的boot盘中的`start4.elf`与`fixup4.dat`用下载的最新固件包中的相同文件替换即可。

![在这里插入图片描述](https://img-blog.csdnimg.cn/95ddba8ae9d84f209b02e7e7dd6260f0.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBASjFGZW5nWg==,size_20,color_FFFFFF,t_70,g_se,x_16)

### 配置网络

https://blog.csdn.net/qq_30613365/article/details/120739069

```shell
cd /etc/netplan
sudo vi 50-cloud-init.yaml
```

这里建议连接手机热点

![img](https://img-blog.csdnimg.cn/c8fba40a2da9411dae5151f34fa1434f.jpg?x-oss-process=image/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBA5bCP55m95aSp5omN,size_20,color_FFFFFF,t_70,g_se,x_16)

```shell
sudo netplan -debug try
sudo netplan -debug generate
sudo netplan -debug apply
```

如果没成功连接wifi，重启就行了

注意：用这种方法连接热点后，安装图形化界面时会导致找不到wifi，此时需要再次进入这个文件，把wifis字段的所有内容注释掉，再执行一下上面三行代码就行了。

### 换源

树莓派上ubuntu的源跟正常的不一样，多了一个ports

```shell
sudo vim /etc/apt/sources.list

# 粘贴到末尾
deb https://mirrors.ustc.edu.cn/ubuntu-ports/ focal main restricted universe multiverse
deb https://mirrors.ustc.edu.cn/ubuntu-ports/ focal-updates main restricted universe multiverse
deb https://mirrors.ustc.edu.cn/ubuntu-ports/ focal-backports main restricted universe multiverse
deb https://mirrors.ustc.edu.cn/ubuntu-ports/ focal-security main restricted universe multiverse


sudo apt-get update
sudo apt-get upgrade
```

### 安装桌面

https://blog.csdn.net/weixin_44415639/article/details/123357377

```shell
sudo apt install ubuntu-desktop
```

此时重启完就有桌面了

### 查看本机ip

```shell
ifconfig # 查看公网ip，进行ssh或vnc连接
```

### 安装RealVNC

https://www.realvnc.com/en/connect/download/vnc/

记得选树莓派arm64版的

```shell
sudo dpkg -i VNC-Server*.deb
```
设置开机自启
```shell
sudo systemctl enable vncserver-x11-serviced.service
```
启动VNC
```shell
sudo systemctl start vncserver-x11-serviced.service
```
