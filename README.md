# UnmannedVehicle
## 设置bash
```
export LD_LIBRARY_PATH=~/mavlink/build/lib:~/ouster_client/build/lib:
```
## 依赖
```
sudo apt-get install ros-foxy-ecl-*
sudo apt install ros-foxy-nav2-*
sudo apt install ros-foxy-joint-state-publisher
```
## 打开底盘
运行
```
sudo chmod 777 /dev/ttyUSB*
ros2 launch aimibot 51-minimal.launch.py
```
## 导航
运行
```
ros2 launch navigation navigation2.launch.py
```
## 雷达
运行
```
ros2 launch ouster_ros ouster.launch.py
```
## 键盘
运行
```
ros2 run py talker
```

# gazebo
```
ros2 launch navigation navigation2.launch.py
ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py
```
