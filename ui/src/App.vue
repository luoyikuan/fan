<script setup lang="ts">
import { ref } from 'vue'
import mqtt from 'mqtt'
import { getConfig } from '@/config/ApplicationCofig'

// 获取配置
const config = getConfig()
console.log(config);

const model = ref({
  rpm: 0,         // 风扇转速
  pwm: 0,         // 风扇调速
  humidity: 0,    // 湿度
  temperature: 0  // 温度
})

// pwd输入标志(防止用户修改pwd时:MCU发来的消息改变进度条)
let pwmInputFlag = false

let mqttClient: mqtt.MqttClient;
// 连接mqtt服务器
mqtt.connectAsync(config.mqtt.url, {
  username: config.mqtt.username,
  password: config.mqtt.password
}).then(client => {
  if (client.connected) {
    ElMessage.success('连接成功!')
    mqttClient = client;

    // 订阅MCU发来的消息
    client.subscribe(config.topic.data)
    client.on('message', (topic, message) => {
      let msg = JSON.parse(message.toString())
      console.log('msg', msg)

      model.value.rpm = msg.rpm
      model.value.humidity = msg.humidity
      model.value.temperature = msg.temperature
      if (!pwmInputFlag) model.value.pwm = msg.pwm
    })

  } else {
    ElMessage.warning('连接失败!')
  }
}).catch(e => {
  console.log(e);
  ElMessage.warning('连接失败:' + e)
})


const pwmChange = (val: number) => {
  pwmInputFlag = false
  if (mqttClient) {
    if (mqttClient.connected) {
      mqttClient.publish(config.topic.ctrl, JSON.stringify({ pwm: val }));
    } else {
      ElMessage.error('MQTT断开连接!')
    }
  } else {
    ElMessage.error('MQTT未连接!')
  }
}

const pwmInput = () => {
  pwmInputFlag = true
}

</script>

<template>
  <header>
    <el-text size="large">眯丑圆智能小屋</el-text>
  </header>

  <main>

    <el-row :gutter="3">

      <!-- 转速 -->
      <el-col :span="8">
        <el-progress type="dashboard" :percentage="model.rpm / 1550 * 100" :indeterminate="true">
          <template #default="{ percentage }">
            <span class="percentage-value">{{ model.rpm }}PRM</span>
            <span class="percentage-label">
              <img src="@/assets/fan.svg" style="width: 23px;" />
            </span>
          </template>
        </el-progress>
      </el-col>

      <!-- 温度 -->
      <el-col :span="8">
        <el-progress type="dashboard" :percentage="model.rpm / 1550 * 100" :indeterminate="true">
          <template #default="{ percentage }">
            <span class="percentage-value">{{ model.rpm }}℃</span>
            <span class="percentage-label">
              <img src="@/assets/temperature.svg" style="width: 25px;" />
            </span>
          </template>
        </el-progress>
      </el-col>

      <!-- 湿度 -->
      <el-col :span="8">
        <el-progress type="dashboard" :percentage="model.rpm / 1550 * 100" :indeterminate="true">
          <template #default="{ percentage }">
            <span class="percentage-value">{{ model.rpm }}%RH</span>
            <span class="percentage-label">
              <img src="@/assets/humidity.svg" style="width: 30px;" />
            </span>
          </template>
        </el-progress>
      </el-col>
    </el-row>

    <div class="ctrl">
      <el-text>转速控制</el-text>
      <el-slider v-model="model.pwm" :max="255" @change="pwmChange" @input="pwmInput" />
    </div>
  </main>
</template>

<style scoped>
header {
  text-align: center;
  margin: 20px 0;
}

main {
  text-align: center;

  .percentage-value {
    display: block;
    margin-top: 10px;
    font-size: 28px;
  }

  .percentage-label {
    display: block;
    margin-top: 10px;
  }

  .ctrl {
    margin: 20px 10px;
    text-align: left;
  }
}
</style>
