<script setup lang="ts">
import {reactive, ref} from 'vue'
import type {FormInstance, FormRules} from 'element-plus'
import {useRouter} from 'vue-router'
import {ElMessage} from 'element-plus'
import {DramaApi, GetCompanyIdByCompanyName} from "@/request/api";
const router = useRouter()

const ruleFormRef = ref<FormInstance>()

const dramaForm = reactive({
  drama_name: '',
  theatre_name: '',
  company_name:'',
})

const submitForm = (formEl: FormInstance | undefined) => {
  if (!formEl) return
  formEl.validate(async (valid) => {
    if (valid) {
      try {
        let cid = await GetCompanyIdByCompanyName({
          company_name: dramaForm.company_name
        })
        console.log(cid)
        let res = await DramaApi({
          drama_name: dramaForm.drama_name,
          theatre_name: dramaForm.theatre_name,
          company_id: cid,
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
      :model="dramaForm"
      style="max-width: 300px"
      label-width="auto"
      class="demo-ruleForm"
  >

    <el-form-item label="剧目" prop="drama_name" >
      <el-input v-model="dramaForm.drama_name" type="text" autocomplete="off" />
    </el-form-item>

    <el-form-item label="剧场" prop="theatre_name">
      <el-input v-model="dramaForm.theatre_name" type="text" autocomplete="off"/>
    </el-form-item>

    <el-form-item label="公司" prop="company_name">
      <el-input v-model="dramaForm.company_name" type="text" autocomplete="off"/>
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