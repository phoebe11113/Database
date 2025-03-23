<script setup lang="ts">
import {onBeforeMount, ref} from "vue";
import {
  GetDramaActorInfoListByActorName,
  GetActorInfoByActorName
} from "@/request/api";
import {useRoute, useRouter} from "vue-router";
import {ElMessage} from "element-plus";



let actor = ref<{ actor_name: string}>({
  actor_name: '',
  birth: '',
  sex: '',
});

const route = useRoute()
interface Dates{
  drama_id: int
  actor_id: int
  drama_name: string
  actor_name: string
  date: Date
}

const tableData = ref<Dates[]>([]);

let currentPage = ref(1);
let pageSize = ref(5);
let total = ref(100);

const router = useRouter()

const handleCurrentChange = (newPage: number) => {
  currentPage.value = newPage;
  fetchData();
};
function formatDate(date) {
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, '0'); // 月份从0开始计数，所以需要+1
  const day = String(date.getDate()).padStart(2, '0');
  const hours = String(date.getHours()).padStart(2, '0');
  const minutes = String(date.getMinutes()).padStart(2, '0');
  const seconds = String(date.getSeconds()).padStart(2, '0');

  return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`;
}

onBeforeMount(async () => {
  try {
    let actor_name = route.params.actor_name;

    let actorInfoRes = await GetActorInfoByActorName({ actor_name });
    actor.value = {
      actor_name: actorInfoRes.actor_name,
      birth: actorInfoRes.birth,
      sex: actorInfoRes.sex,
    };

    let dramaListRes = await GetDramaActorInfoListByActorName({
      actor_name: actor_name,
      skip: (currentPage.value - 1) * pageSize.value,
      limit: pageSize.value,
    });

    tableData.value = dramaListRes.dates.map(item => ({ // 假设数据在 response 的 data 属性中
      drama_id: item.drama_id,
      actor_id: item.actor_id,
      drama_name: item.drama_name,
      actor_name: item.actor_name,
      date: formatDate(new Date(item.date))
    }));

    total.value = Math.ceil(dramaListRes.total / pageSize.value); // 确保响应中包含 total

  } catch (e) {
    console.error(e);
    ElMessage.error('信息获取失败');
  }
});




</script>

<template>
  <div class="actor-profile">
    <!--    <img :src="company.avatar" alt="Company Avatar" class="avatar"/>-->
    <h2>{{ actor.actor_name }}</h2>
    <!--    <p><strong>IP:</strong> {{ company.company_ip }} </p>-->
    <p><strong>Birth:</strong> {{ actor.birth }}</p>
    <p><strong>Sex:</strong> {{ actor.sex }}</p>
  </div>
  <el-row>
    <el-col :span="24">
      <el-table :data="tableData" stripe style="width: 50%;margin-top:  60px;margin-left: 400px">
<!--        <el-table-column prop="drama_id" label="剧目序号" width="200"/>-->
        <el-table-column prop="drama_name" label="剧目名" width="200"/>
<!--        <el-table-column prop="actor_id" label="演员序号" width="200"/>-->
        <el-table-column prop="actor_name" label="演员名" width="200"/>
        <el-table-column prop="date" label="演出日期" width="200"/>
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
.actor-profile {
  max-width: 300px;
  margin: 20px auto;
  padding: 20px;
  border: 1px solid #ccc;
  border-radius: 10px;
  text-align: center;
}

.custom-pagination{
  margin-left: 400px
}
</style>