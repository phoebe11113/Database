<script setup lang="ts">
import {reactive, ref} from 'vue'
import type {FormInstance, FormRules} from 'element-plus'
import {useRouter} from 'vue-router'
import {ElMessage} from 'element-plus'
import {DateApi, GetIdByName} from "@/request/api";
const router = useRouter()

const ruleFormRef = ref<FormInstance>()

const initForm = reactive({
  drama_name:'',
  actor_name:'',
  date:'',
})

// const dateForm = reactive({
//   actor_id: '',
//   drama_id: '',
//   date:'',
// })

const submitForm = (formEl: FormInstance | undefined) => {
  if (!formEl) return
  formEl.validate(async (valid) => {
    if (valid) {
      try {
        let dateForm = await GetIdByName({
          drama_name: initForm.drama_name,
          actor_name: initForm.actor_name,
        })
        let res = await DateApi({
          actor_id: dateForm.actor_id,
          drama_id: dateForm.drama_id,
          date: initForm.date,
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
      :model="initForm"
      style="max-width: 300px"
      label-width="auto"
      class="demo-ruleForm"
  >

    <el-form-item label="剧目名称" prop="drama_name" >
      <el-input v-model="initForm.drama_name" type="text" autocomplete="off" />
    </el-form-item>

    <el-form-item label="演员姓名" prop="actor_name">
      <el-input v-model="initForm.actor_name" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="排期" prop="date">
      <!-- 使用 el-date-picker 代替 el-input -->
      <el-date-picker
          v-model="initForm.date"
          type="datetime"
          placeholder="选择日期和时间"
          format="YYYY-MM-DD HH:mm:ss"
          value-format="YYYY-MM-DD HH:mm:ss"
      />
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