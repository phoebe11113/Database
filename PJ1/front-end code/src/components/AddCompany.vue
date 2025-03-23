<script setup lang="ts">
import {reactive, ref} from 'vue'
import type {FormInstance, FormRules} from 'element-plus'
import {useRouter} from 'vue-router'
import {ElMessage} from 'element-plus'
import {CompanyApi} from "@/request/api";
const router = useRouter()

const ruleFormRef = ref<FormInstance>()

const companyForm = reactive({
  company_name: '',
  company_ip: '',
})

const submitForm = (formEl: FormInstance | undefined) => {
  if (!formEl) return
  formEl.validate(async (valid) => {
    if (valid) {
      try {
        let res = await CompanyApi({
          company_name: companyForm.company_name,
          company_ip: companyForm.company_ip,
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
      :model="companyForm"
      style="max-width: 300px"
      label-width="auto"
      class="demo-ruleForm"
  >

    <el-form-item label="公司名" prop="company_name" >
      <el-input v-model="companyForm.company_name" type="text" autocomplete="off" />
    </el-form-item>

    <el-form-item label="公司IP" prop="company_ip">
      <el-input v-model="companyForm.company_ip" type="text" autocomplete="off"/>
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