<script setup lang="ts">
import {
  Avatar, CirclePlus, CirclePlusFilled,
  Document,
  Menu as IconMenu, Mic, MilkTea,
  Setting, VideoCamera,
} from '@element-plus/icons-vue'
import {useRoute, useRouter} from 'vue-router'
import {computed} from "vue";
import {useUserstore} from '@/store/user'

const userStore=useUserstore()
const router = useRouter()
const route = useRoute()


const isAdmin = userStore.userName == "phoebe"

const isAdminUser = computed( ()=> (userStore.userName== "phoebe") )
console.log(isAdminUser.value)

function change(key: string, keyPath: string[]) {
  console.log(key)
}
</script>

<template>
  <el-menu
      default-active="1"
      class="el-menu-vertical-demo"
      :router=true
  >
    <el-menu-item index="/index/">
      <el-icon>
        <icon-menu/>
      </el-icon>
      <span>个人信息</span>
    </el-menu-item>
    <el-menu-item index="/index/checkUserInfo">
      <el-icon>
        <milk-tea/>
      </el-icon>
      <span>所有用户</span>
    </el-menu-item>
    <el-menu-item index="/index/checkDramaInfo">
      <el-icon>
        <video-camera/>
      </el-icon>
      <span>所有剧目</span>
    </el-menu-item>
    <el-menu-item index="/index/checkActorInfo">
      <el-icon>
        <mic/>
      </el-icon>
      <span>所有演员</span>
    </el-menu-item>
<!--    <el-menu-item index="/index/checkCompanyInfo">-->
<!--      <el-icon>-->
<!--        <mic/>-->
<!--      </el-icon>-->
<!--      <span>所有公司</span>-->
<!--    </el-menu-item>-->
    <el-menu-item index="/index/addUser">
      <el-icon>
        <Avatar/>
      </el-icon>

      <span>添加用户</span>
    </el-menu-item>
    <el-menu-item index="/index/addDrama" v-if = "isAdmin">
      <el-icon>
        <CirclePlus/>
      </el-icon>
      <span>添加剧目</span>
    </el-menu-item>
     <el-menu-item index="/index/addActor" v-if = "isAdmin">
          <el-icon>
            <CirclePlusFilled/>
          </el-icon>
          <span>添加演员</span>
        </el-menu-item>
    <el-menu-item index="/index/addCompany" v-if = "isAdmin">
          <el-icon>
            <CirclePlus/>
          </el-icon>
          <span>添加公司</span>
        </el-menu-item>
    <el-menu-item index="/index/addDate" v-if = "isAdmin">
          <el-icon>
            <CirclePlusFilled/>
          </el-icon>
          <span>添加排期</span>
        </el-menu-item>
  </el-menu>
</template>

<style scoped>
</style>