<script setup lang="ts">
import {reactive, ref} from 'vue'
import type {FormInstance, FormRules} from 'element-plus'
import {useRouter} from 'vue-router'
import {ElMessage} from 'element-plus'
import {ActorApi} from "@/request/api";
const router = useRouter()

const ruleFormRef = ref<FormInstance>()

const actorForm = reactive({
  actor_name: '',
  birth: '',
  sex:'',
})

const submitForm = (formEl: FormInstance | undefined) => {
  if (!formEl) return
  formEl.validate(async (valid) => {
    if (valid) {
      try {
        let res = await ActorApi({
          actor_name: actorForm.actor_name,
          birth: actorForm.birth,
          sex: actorForm.sex,
        })
        ElMessage.success('添加成功')
      } catch (e) {
        console.log(e)
        ElMessage.error('添加失败请重新输入')
      }
      // if (res.success) {
      //   ElMessage.success('注册成功')
      //   await router.push('/');
      // } else {
      //   ElMessage.error('注册失败请重新输入')
      // }
    } else {
      ElMessage.error('添加失败请重新输入')
      return false
    }
  })
}


</script>

<template>
  <el-form
      ref="ruleFormRef"
      :model="actorForm"
      style="max-width: 300px"
      label-width="auto"
      class="demo-ruleForm"
  >

    <el-form-item label="演员" prop="actor_name" >
      <el-input v-model="actorForm.actor_name" type="text" autocomplete="off" />
    </el-form-item>

    <el-form-item label="生日" prop="birth">
      <el-input v-model="actorForm.birth" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="性别" prop="sex">
      <el-input v-model="actorForm.sex" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item>
      <el-button type="success" @click="submitForm(ruleFormRef)"
      >添加
      </el-button
      >
    </el-form-item>

  </el-form>
</template>

<style scoped>

</style>