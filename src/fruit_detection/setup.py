from setuptools import find_packages, setup

package_name = 'fruit_detection'

setup(
    name=package_name,
    version='0.0.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='貝淵 蒼馬',
    maintainer_email='Raptor-S@outlook.jp',
    description='高専ロボコン 2023 では、つり下がっているフルーツを回収するのが競技課題である。ロボットに載せてあるカメラから、YOLO V8 でリアルタイムでフルーツを検出し、回収装置(お助けアイテム)の位置を調整する。フルーツの座標を ROS2 humble で publish する。',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'pubdetect = fruit_detection.pubdetect:main',
            'talker = fruit_detection.publisher_member_function:main',
            'listener = fruit_detection.subscriber_member_function:main',
        ],
    },
)
