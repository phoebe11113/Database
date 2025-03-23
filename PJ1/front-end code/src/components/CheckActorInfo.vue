<script setup lang="ts">
import {onBeforeMount, ref} from "vue";
import {GetActorInfoList, GetDramaInfoList} from "@/request/api";
import {useRouter} from "vue-router";

interface Actor{
  actor_name: string
  birth: string
  sex: string
}

const tableData = ref<Actor[]>([]);

let currentPage = ref(1);
let pageSize = ref(10);
let total = ref(100);
let searchActorName = ref('');

const router = useRouter()

const fetchData = async (searchName?: string) => {
  let res = await GetActorInfoList({
    skip: (currentPage.value - 1) * pageSize.value,
    limit: pageSize.value,
    actor_name: searchName || "" // 如果 searchName 存在，则作为查询条件，否则为空字符串
  });
  tableData.value = res.actors.map(item => ({
    actor_name: item.actor_name,
    birth: item.birth,
    sex: item.sex
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

const handleClick = (actor_name: string) => {
  console.log(actor_name)
  router.push({name: 'checkActorDetail', params: {actor_name: actor_name}})
};
// 搜索功能
const handleSearch = async (searchName: string) => {
  currentPage.value = 1; // 重置到第一页
  await fetchData(searchName);
  await router.push({name: 'checkActorDetail', params: {actor_name: searchName}});
};


</script>

<template>
  <div class="actor-profile">
    <el-input
        v-model="searchActorName"
        placeholder="请输入演员名"

        @keyup.enter="handleSearch(searchActorName)"
        clearable
    />
  <el-row>
    <el-col :span="24">
      <el-table :data="tableData" stripe style="width: 100%">
        <el-table-column prop="actor_name" label="姓名" width="200"/>
        <el-table-column prop="birth" label="生日" width="200"/>
        <el-table-column prop="sex" label="性别" width="200"/>
        <el-table-column fixed="right" label="演出信息" min-width="120">
          <template v-slot:default="scope">
            <el-button link type="primary" size="small" @click="handleClick(scope.row.actor_name)">
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