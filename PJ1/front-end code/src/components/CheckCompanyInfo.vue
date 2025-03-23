<script setup lang="ts">
import {onBeforeMount, ref} from "vue";
import {GetCompanyIdByCompanyName, GetCompanyInfoList} from "@/request/api";
import {useRouter} from "vue-router";

interface Company{
  company_name: string
  company_ip: string
}
const tableData = ref<Company[]>([]);

let currentPage = ref(1);
let pageSize = ref(10);
let total = ref(100);
let searchCompanyName = ref('');

const router = useRouter()

const fetchData = async (searchName?: string) => {
  let res = await GetCompanyInfoList({
    skip: (currentPage.value - 1) * pageSize.value,
    limit: pageSize.value,
    company_name: searchName || "" // 如果 searchName 存在，则作为查询条件，否则为空字符串
  });
  tableData.value = res.companies.map(item => ({
    company_name: item.company_name,
    company_ip: item.company_ip
  }));
  total.value = Math.ceil(res.total / pageSize.value);
};

onBeforeMount(async () => {
  await fetchData(); // 组件挂载时初始化数据
});


const handleCurrentChange = (newPage: number) => {
  currentPage.value = newPage;
  fetchData();
};

const handleClick = async (company_name: string) => {
  try {
    let companyId = await GetCompanyIdByCompanyName({ company_name: company_name });
    if (companyId) {
      await router.push({ name: 'checkCompanyDetail', params: { company_id: companyId } });
    } else {
      console.error('Failed to retrieve company ID');
    }
  } catch (error) {
    console.error('Error handling click:', error);
  }
};

const handleSearch = async (searchName: string) => {
  currentPage.value = 1;
  try {
    await fetchData(searchName);
    let response = await GetCompanyIdByCompanyName({ company_name: searchName });
    let companyId = response.data?.company_id;
    if (companyId) {
      await router.push({ name: 'checkCompanyDetail', params: { company_id: companyId } });
    } else {
      console.error('Failed to retrieve company ID for search');
    }
  } catch (error) {
    console.error('Error handling search:', error);
  }
};

</script>

<template>
  <div class="company-profile">
    <el-input
        v-model="searchCompanyName"
        placeholder="请输入公司名"
        @keyup.enter="handleSearch(searchCompanyName)"
        clearable
    />
    <el-row>
      <el-col :span="24">
        <el-table :data="tableData" stripe style="width: 100%">
          <el-table-column prop="company_name" label="公司名称" width="200"/>
          <el-table-column prop="company_ip" label="公司ip" width="200"/>
          <el-table-column fixed="right" label="公司信息" min-width="120">
            <template v-slot:default="scope">
              <el-button link type="primary" size="small" @click="handleClick(scope.row.company_name)">
                Detail
              </el-button>
            </template>
          </el-table-column>
        </el-table>
      </el-col>
    </el-row>

    <el-pagination
        @current-change="handleCurrentChange"
        :current-page="currentPage"
        :page-size="pageSize"
        layout="prev, pager, next"
        :total="total"
    />
  </div>
</template>

<style scoped>

</style>