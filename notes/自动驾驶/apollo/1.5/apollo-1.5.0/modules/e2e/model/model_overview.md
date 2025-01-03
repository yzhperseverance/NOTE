# Steering Model

The steering wheel control model is computing curvature.

`code file：run_model.py`
## Generator Data

```
def generator_data_steering():
```

Filter the abnormal values：

```
def filter(c,v):
	return v > 1 and c < 0.5
	
```
Sequentially generator the data, due to the collected data contains a large number of straight lines data(curvature less than or equal to 0.0001). After filtering the abnormal values, select the straight line data by probability of 10%.

```
if filter(curv, speed) and (curv > 0.0001 or random.randint(1, 10) == 1)
```
## CNN Model
`model file：src\steering_model.py`

<img src="./png_resource/steering_model1.png" width = "300" height = "320" align=center />
<img src="./png_resource/steering_model2.png" width = "300" height = "320" align=center />
<img src="./png_resource/steering_model3.png" width = "300" height = "320" align=center />



# Longitudinal Model
The longitudinal model is computing the acceleration.

`code file：run_model.py`
## Generator Data

```
def generator_data_acc():
```

Filter the abnormal values：

```
def filter(c,v):
	return v > 1 and c < 0.5
	
```
Sequentially read the data, defines time_step = 5, filters the abnormal datas, selects the five-frame image of the order, and compute the acceleration value at the time of the fifth frame as the input.Each frame interval of 0.125s, the fifth frame of the acceleration:

<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=default"></script>

$$ ACC_5 = \frac{(V_6-V_4)}\{0.25} $$


If the sixth frame velocity value is filtered out by the filter, then discard the sample of the five frames.

```
next_curv = attrs[i+1][4]
next_speed = math.sqrt(attrs[i+1][1] ** 2 + attrs[i+1][2] ** 2)
if not filter(next_curv, next_speed):
	step_cnt = 0
	continue
else:
	acc = (next_speed - step_v[3]) / t_sep

```

## Conv_LSTM模型
`model file:src\acc_model.py`

<img src="./png_resource/acc_model.png" width = "300" height = "320" align=center />

