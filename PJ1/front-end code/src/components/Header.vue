<script setup lang="ts">
import {ElMessage, ElNotification as notify} from 'element-plus'
import {LogoutApi} from "@/request/api";
import {useRouter} from 'vue-router'
import {useUserstore} from "@/store/user";
const router = useRouter()
const userStore=useUserstore()
async function logout() {
  userStore.userName = 'userName'
  userStore.token = 'token'
  ElMessage.success("登出成功")
  await router.push('/')
  // let res = await LogoutApi()
  // if (res.success) {
  //   ElMessage.success("登出成功")
  //   await router.push('/')
  // } else {
  //   ElMessage("登出失败")
  // }
  // console.log(res)
}
</script>

<template>
  <div style="display: flex; justify-content: space-between; align-items: center;">
    <img src='@/assets/images/logo1.png' alt="Logo" style="height:50px;margin-top: 10px;">
    <div>
      <el-button type="info" @click="logout">登出</el-button>
    </div>
  </div>
</template>

<style scoped>

</style>