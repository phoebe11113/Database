<script setup lang="ts">
import {onBeforeMount, ref} from "vue";
import {GetDramaInfoListByCompany} from "@/request/api";
import {useRoute, useRouter} from "vue-router";
import {useCompanystore} from "@/store/company";
import {GetCompanyInfoByCompanyId} from "@/request/api";
import {ElMessage} from "element-plus";


const CompanyStore=useCompanystore()

let company = ref({
  company_name: '',
  company_ip: '',
  // avatar: 'https://tse4-mm.cn.bing.net/th/id/OIP-C.ffVjXyf76-70IQYd75H7wgAAAA?rs=1&pid=ImgDetMain', // 假设的头像URL
})


const route = useRoute()
interface Drama{
  drama_name: string
  theatre_name: string
  company_id: int
}

const tableData = ref<Drama[]>([]);

let currentPage = ref(1);
let pageSize = ref(5);
let total = ref(100);

const router = useRouter()

const handleCurrentChange = (newPage: number) => {
  currentPage.value = newPage;
  fetchData();
};


onBeforeMount(async () => {
  try {
    let company_id = CompanyStore.company_id
    console.log(route.params)
    if ('company_id' in route.params) {
      console.log(route.params.company_id)
      company_id = route.params.company_id
    }
    let res = await GetCompanyInfoByCompanyId({
      company_id: company_id
    })
    console.log(res)
    company.value.company_name = res.company_name
    company.value.company_ip = res.company_ip

  } catch (e) {
    console.log(e)
    ElMessage.error('公司信息查询失败')

  }
  let company_id = route.params.company_id
  let res = await GetDramaInfoListByCompany({
    company_id: company_id,
    skip: (currentPage.value - 1) * pageSize.value,
    limit: pageSize.value,
  })
  tableData.value = res.dramas.map(item => ({
    drama_name: item.drama_name,
    theatre_name: item.theatre_name,
    company_id: item.company_id
  }));

  total.value = Math.ceil(res.total / pageSize.value)
  console.log('total_pages.value:',total.value)

});




</script>

<template>
  <div class="company-profile">
    <!--    <img :src="company.avatar" alt="Company Avatar" class="avatar"/>-->
    <h2>{{ company.company_name }}</h2>
    <p><strong>IP:</strong> {{ company.company_ip }} </p>
    <!--    <p><strong>Email:</strong> {{ user.email }}</p>-->
  </div>
  <el-row>
    <el-col :span="24">
      <el-table :data="tableData" stripe style="width: 50%;margin-top:  60px;margin-left: 350px">
        <el-table-column prop="drama_name" label="姓名" width="200"/>
        <el-table-column prop="theatre_name" label="剧场" width="350"/>
        <el-table-column prop="company_id" label="公司id" width="100"/>
      </el-table>
    </el-col>
  </el-row>

  <el-pagination
      @current-change="handleCurrentChange"
      :current-page="currentPage"
      :page-size="pageSize"
      layout="prev, pager, next"
      :total="total"
      class="custom-pagination"
  />

</template>

<style scoped>
.company-profile {
  max-width: 300px;
  margin: 20px auto;
  padding: 20px;
  border: 1px solid #ccc;
  border-radius: 10px;
  text-align: center;
}

.custom-pagination{
  margin-left: 350px
}

</style>