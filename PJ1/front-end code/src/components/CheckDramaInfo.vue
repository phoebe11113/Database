<script setup lang="ts">
import {onBeforeMount, ref} from "vue";
import {GetDramaInfoList} from "@/request/api";
import {useRouter} from "vue-router";

interface Drama{
  drama_name: string
  theatre_name: string
  company_id: int
}



const tableData = ref<Drama[]>([]);

let currentPage = ref(1);
let pageSize = ref(10);
let total = ref(100);
let searchDramaName = ref('');

const router = useRouter()

// onBeforeMount(async () => {
//   let res = await GetDramaInfoList({
//     skip: (currentPage.value - 1) * pageSize.value,
//     limit: pageSize.value,
//   })
//   res.dramas.forEach(item => {
//     tableData.value.push({
//       drama_name: item.drama_name,
//       theatre_name: item.theatre_name,
//       company_id: item.company_id
//     });
//   });
//   total.value = Math.ceil(res.total / pageSize.value)
//   console.log('total_pages.value:',total.value)
// })
const fetchData = async (searchName?: string) => {
  let res = await GetDramaInfoList({
    skip: (currentPage.value - 1) * pageSize.value,
    limit: pageSize.value,
    drama_name: searchName || "" // 如果 searchName 存在，则作为查询条件，否则为空字符串
  });
  tableData.value = res.dramas.map(item => ({
    drama_name: item.drama_name,
    theatre_name: item.theatre_name,
    company_id: item.company_id
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

const handleClickCompany = (company_id: int) => {
  console.log(company_id)
  router.push({name: 'checkCompanyDetail', params: {company_id: company_id}})
};
const handleClickDrama = (drama_name: str) => {
  console.log(drama_name)
  router.push({name: 'checkDramaDetail', params: {drama_name: drama_name}})
};
// 搜索功能
const handleSearch = async (searchName: string) => {
  currentPage.value = 1; // 重置到第一页
  await fetchData(searchName);
  await router.push({ name: 'checkDramaDetail', params: { drama_name: searchName } });
};


</script>

<template>
  <div class="drama-profile">
    <el-input
        v-model="searchDramaName"
        placeholder="请输入剧名"
        clearable
        @keyup.enter="handleSearch(searchDramaName)"
    />
    <el-row>
    <el-col :span="24">
      <el-table :data="tableData" stripe style="width: 100%">
        <el-table-column prop="drama_name" label="姓名" width="200"/>
        <el-table-column prop="theatre_name" label="剧场" width="360"/>
        <el-table-column prop="company_id" label="公司id" width="200"/>
        <el-table-column fixed="right" label="公司信息" min-width="120">
          <template v-slot:default="scope">
            <el-button link type="primary" size="small" @click="handleClickCompany(scope.row.company_id)">
              Company Information
            </el-button>
          </template>
        </el-table-column>
        <el-table-column fixed="right" label="演出信息" min-width="120">
          <template v-slot:default="scope">
            <el-button link type="primary" size="small" @click="handleClickDrama(scope.row.drama_name)">
              Details
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