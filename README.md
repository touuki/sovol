# Sovol
## 使用说明

### 单位
程序中所采用的是归一化的相对单位，即：  
+ 电荷单位：`e`  
+ 速度单位：`c`  
+ 质量单位：`m_e`  

根据上面的归一化可以导出：  
+ 能量单位：`{m_e}c^2`  
+ 动量单位：`{m_e}c`  

上面所做的归一化等价于令所有公式中的`e = c = m_e = 1`.  
如果选取一个参考频率`ω`，还可以做如下的归一化：  
+ 时间单位：`1/ω`  
+ 长度单位：`c/ω`  
+ 电场强度单位：`{m_e}cω/e`  
+ 磁感应强度单位：`{m_e}ω/e`   
+ 电流密度单位：`{m_e}ω^2/{μ_0}ce`
+ 电流强度单位：`{m_e}c/{μ_0}e`

注意这个`ω`是可以自由选取的，你可以将模拟结果解释到任意尺度。对于激光这个参考频率可以选择为激光的`ω`，这样就相当于令`ω = k = 1`，不过要注意这种解释下波长为`2π`，激光周期也为`2π`.   

这种归一化好处是可以减少公式的计算量，并且不用考虑SI单位制与高斯单位制的区别。

### 参数设置
#### 参考频率 `reference_frequency`
　当使用到辐射相关Algorithm时需要。

#### 控制参数 `control`
+ `total_particle_number` (**Integer**; *>0*; default: *1*)  
　模拟粒子总数
+ `time_step` (**Float**; *>0*)  
　时间步长
+ `data_start_time` (**Float**; *>=0*; default: *0*)  
　开始输出数据时刻
+ `data_end_time` (**Float**; *>0*)  
　结束时刻
+ `data_interval` (**Integer**)  
　每隔多少个时间步长输出一次数据。如果值小于等于0，则只输出`data_start_time`及`data_end_time`两个时刻的数据
+ `parallel_workers` (**Integer**; *>0*; default: *1*; only for CLI)  
　并行模拟线程数
+ `table_dirname` (**String**; only for CLI)  
　数值表所在目录
+ `output_filename` (**String**; default *result-p${pid}.h5*; only for CLI)  
　数据储存H5文件名
+ `output_items` (**String Array**; default: *all available items*; only for CLI)  
　需要储存的数据项，可用项有：x, y, z, px, py, pz, sx, sy, sz, Ex, Ey, Ez, Bx, By, Bz, optical_depth
+ `data_per_frame` (**Integer**; *>0*; only for WASM)  
　多少个数据刷新一帧
+ `frames_per_second` (**Integer**; *>0*; only for WASM)  
　每秒多少帧

#### 电磁场 `field`
+ `name` (**String**)  
　Field类名，见[可用类-Field](#Field)
+ 其他类属性

#### 粒子生产器 `particle_producer`
+ `particle` (**Particle**)  
　基础粒子
    + `type` (**Integer**; *0 or 1*; default: *0*)  
    　*0*为普通粒子；*1*为电子，可处理辐射、自旋等
    + `mass` (**Float**; *>0*; only for `type==0`)  
    　粒子质量
    + `charge` (**Float**; only for `type==0`)  
    　粒子电量
    + `position` (**Float Array[3]**; default: *[0,0,0]*)  
    　粒子位置
    + `momentum` (**Float Array[3]**; default: *[0,0,0]*)  
    　粒子动量
    + `polarization` (**Float Array[3]**; *length<=1*; default: *[0,0,0]*)  
    　粒子极化度
+ `shifters` (**ParticleShifter Array**)  
　粒子变换器，按顺序依次作用在基础粒子上
    + `name` (**String**)  
    　ParticleShifter类名，见[可用类-ParticleShifter](#ParticleShifter)
    + 其他类属性

#### 相互作用算法 `algorithm`
+ `name` (**String**)  
　Algorithm类名，见[可用类-Algorithm](#Algorithm)
+ 其他类属性

### 可用类
#### Field
##### CustomField
　自定义函数的电磁场  
CLI版本：
+ `global_function_name` (**String**)  
　自定义电磁场的Lua全局函数名。其接收参数为x, y, z, t，返回值结构为`{E={Ex,Ey,Ez},B={Bx,By,Bz}}`

WASM版本：
+ `func` (**Function**)  
　自定义电磁场函数。其接收参数为x, y, z, t，返回值结构为`{E:[Ex,Ey,Ez],B:[Bx,By,Bz]}`
##### LaguerreGaussianPulseField
　拉盖尔高斯型激光脉冲的电磁场，激光轴线为z轴，沿z轴正向传播，腰为`z=0`平面
+ `a0` (**Float**; *>0*)  
　激光强度归一化参数。对于具有相同a0、w0和tau的不同模式及偏振的激光，它们具有相同的能量，对于基态（`l=0`, `p=0`）线偏振（`polar=0 or ±1`）模式这代表着腰处的矢势强度
+ `tau` (**Float**; *>0*)  
　激光脉冲长度。激光能量的时域半高全宽，时域为sech型包络
+ `w0` (**Float**; *>0*)  
　激光腰半径。对于基态模式（`l=0`, `p=0`）激光在腰半径处的场强降为中心处的*1/e*
+ `polar` (**Float**; *abs<=1*; default: 0)  
　激光偏振模式。*0*为电场沿x轴方向的线偏振，*±1*为电场沿y轴方向的线偏振，*√2/2*为右旋圆偏振（面向激光观察），*-√2/2*为左旋圆偏振
+ `delay` (**Float**; default: 0)  
　激光脉冲延迟。为*0*则当模拟开始时（`t=0`）激光脉冲的时域中心（峰值）位于激光的腰上（`z=0`），*<0*则在模拟开始时处于发散状态（`z>0`），*>0*在模拟开始时处于汇聚状态（`z<0`）
+ `p` (**Integer**; *>=0*; default: 0)  
　拉盖尔高斯模式径向系数。具体参见维基词条[Gaussian beam](https://en.wikipedia.org/wiki/Gaussian_beam#Laguerre-Gaussian_modes)
+ `l` (**Integer**; default: 0)  
　拉盖尔高斯模式周向系数。具体参见维基词条[Gaussian beam](https://en.wikipedia.org/wiki/Gaussian_beam#Laguerre-Gaussian_modes)
+ `iphase` (**Float**; default: 0)  
　模拟开始时刻（`t=0`）的激光相位，主要用以调解少周期激光的载波包络相位。为*0*时对应模拟开始时刻腰处（`z=0`）的相位为*Ey*
+ `k` (**Float**; *>0*; default: 1)  
　激光波数。为*1*时对应激光波长及周期为2π
+ `h` (**Float**; *>0*; default: 0.1)  
　高阶模式（`p>0`）用以计算纵向电磁场的差分步长，以激光波长为单位
##### FieldWithShifters
　在某一Field类上施加变换从而获得新的电磁场
+ `field` (**Field**)  
　基础电磁场
    + `name` (**String**)  
    　Field类名，见[可用类-Field](#Field)
    + 其他类属性
+ `shifters` (**FieldShifter Array**)  
　电磁场变换器，按顺序依次作用在基础电磁场上
    + `name` (**String**)  
    　FieldShifter类名，见[可用类-FieldShifter](#FieldShifter)
    + 其他类属性
##### FieldCombiner
　将多个Field类结合
+ `fields` (**Field Array**)  
　基础电磁场
    + `name` (**String**)  
    　Field类名，见[可用类-Field](#Field)
    + 其他类属性

#### FieldShifter
##### FieldTranslator
　将电磁场进行平移操作
+ `displacement` (**Float Array[3]**)  
　位移量
##### FieldRotator
　将电磁场绕某一点旋转
+ `center` (**Float Array[3]**; default: *[0,0,0]*)  
　旋转中心
+ `rotator` (**Rotator**)  
　旋转器
    + `name` (**String**)  
    　Rotator类名，见[可用类-Rotator](#Rotator)
    + 其他类属性

#### ParticleShifter
##### CustomParticleShifter
　自定义粒子变换器  
CLI版本：
+ `global_function_name` (**String**)  
　自定义粒子变换器的Lua全局函数名。其接收参数为一个包含属性`type`, `mass`, `charge`, `position`, `momentum` 及 `polarization`的粒子结构，返回值结构需包含变换后的`position`, `momentum` 及 `polarization`属性

WASM版本：
+ `func` (**Function**)  
　自定义粒子变换器函数。其接收参数为一个包含属性`type`, `mass`, `charge`, `position`, `momentum`, `polarization`的粒子结构，返回值结构需包含变换后的`position`, `momentum` 及 `polarization`属性
##### ParticleTranslator
将粒子进行平移
+ `displacement` (**Float Array[3]**)  
　位移量
##### ParticleRotator
　将粒子绕某一点旋转
+ `center` (**Float Array[3]**; default: *[0,0,0]*)  
　旋转中心
+ `rotator` (**Rotator**)  
　旋转器
    + `name` (**String**)  
    　Rotator类名，见[可用类-Rotator](#Rotator)
    + 其他类属性
+ `affect_position` (**Boolean**; default: *true*)  
　是否应用于粒子位置
+ `affect_momentum` (**Boolean**; default: *true*)  
　是否应用于粒子动量
+ `affect_polarization` (**Boolean**; default: *true*)  
　是否应用于粒子极化

#### Rotator
##### IntrinsicRotator
　内在旋转旋转器。依次进行yaw、pitch及roll旋转操作
+ `alpha` (**Float**; default: *0*)  
　yaw旋转角度
+ `beta` (**Float**; default: *0*)  
　pitch旋转角度
+ `gamma` (**Float**; default: *0*)  
　roll旋转角度
##### ExtrinsicRotator
　外在旋转旋转器。依次绕x轴、y轴和z轴进行旋转
+ `alpha` (**Float**; default: *0*)  
　绕x轴旋转角度
+ `beta` (**Float**; default: *0*)  
　绕y轴旋转角度
+ `gamma` (**Float**; default: *0*)  
　绕z轴旋转角度

#### Algorithm
##### NoneAlgorithm
　对粒子无效果，可用于诊断电磁场等

##### RungeKuttaAlgorithm
　RK4算法，只适用于普通带电粒子在电磁场中的运动过程，无法处理电子自旋进动等

##### LeapfrogAlgorithm
　蛙跳算法，可处理电子经典自旋演化效应

##### ModifiedAMMLeapfrogAlgorithm
　修正了反常磁矩的蛙跳算法，通过数值表对不同电子参量下的反常磁矩进行了修正，需要设置全局`reference_frequency`

##### MonteCarloRadiativePolarizationAlgorithm
　蒙特卡罗辐射极化算法，通过蒙特卡罗方法辐射光子，并且计算辐射对电子的反作用以及极化度影响，需要设置全局`reference_frequency`
+ `base_algorithm` (**Algorithm**)  
　基础算法，用以处理粒子基础运动过程
    + `name` (**String**)  
    　Algorithm类名，见[可用类-Algorithm](#Algorithm)
    + 其他类属性
+ `min_chi_e` (**Float**; *>=0*; default: *0*)  
　产生效应的最低电子参量，若低于此值则直接跳过算法
+ `disable_reaction` (**Boolean**; default: *false*)  
　是否禁用辐射对电子动量反作用
+ `disable_spin_effect` (**Boolean**; default: *false*)  
　是否禁止辐射对电子自旋极化产生影响
##### ContinuousRadiativePolarizationAlgorithm
　连续辐射极化算法，通过半经典的考虑辐射对电子的累积效应，计算辐射对电子的反作用以及极化度影响，需要设置全局`reference_frequency`
+ `base_algorithm` (**Algorithm**)  
　基础算法，用以处理粒子基础运动过程
    + `name` (**String**)  
    　Algorithm类名，见[可用类-Algorithm](#Algorithm)
    + 其他类属性
+ `min_chi_e` (**Float**; *>=0*; default: *0*)  
　产生效应的最低电子参量，若低于此值则直接跳过算法
+ `disable_reaction` (**Boolean**; default: *false*)  
　是否禁用辐射对电子动量反作用
+ `disable_spin_effect` (**Boolean**; default: *false*)  
　是否禁止辐射对电子自旋极化产生影响
