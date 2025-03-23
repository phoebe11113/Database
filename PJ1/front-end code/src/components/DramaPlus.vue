<script setup lang="ts">
import {onBeforeMount, ref} from "vue";
import {
  GetDramaActorInfoListByDramaName,
  GetDramaInfoByDramaName
} from "@/request/api";
import {useRoute, useRouter} from "vue-router";
import {ElMessage} from "element-plus";



let drama = ref<{ drama_name: string; drama_id: int }>({
  drama_name: '',
});

const route = useRoute()
interface Dates_group{
  drama_id: number
  drama_name: string
  actor_names: string[]
  date: string
}

const tableData = ref<Dates_group[]>([]);

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
  var formattedDateTime;
  try {
    let drama_name = route.params.drama_name || CompanyStore.drama_name;

    let dramaInfoRes = await GetDramaInfoByDramaName({drama_name});
    drama.value = {
      drama_name: dramaInfoRes.drama_name,
      theatre_name: dramaInfoRes.theatre_name,
      drama_id: dramaInfoRes.drama_id,
    };

    let actorListRes = await GetDramaActorInfoListByDramaName({
      drama_name: drama_name,
      skip: (currentPage.value - 1) * pageSize.value,
      limit: pageSize.value,
    });

    if (Array.isArray(actorListRes.dates_group)) {
      tableData.value = actorListRes.dates_group.map(item => ({
        drama_id: item.drama_id,
        drama_name: item.drama_name,
        actor_names: Array.isArray(item.actor_names) ? item.actor_names : [item.actor_names],
        date: formatDate(new Date(item.date))
      }));

      total.value = Math.ceil(actorListRes.total || 0 / pageSize.value); // 确保 actorListRes.total 是数字，如果不是则默认为 0
    } else {
      console.error('actorListRes.dates_g is not an array');
      ElMessage.error('数据格式错误，无法加载演员信息');
    }

  } catch (e) {
    console.error(e);
    ElMessage.error('信息获取失败');
  }
});




</script>

<template>
  <div class="drama-profile">
    <!--    <img :src="company.avatar" alt="Company Avatar" class="avatar"/>-->
    <h2>{{ drama.drama_name }}</h2>
<!--    <p><strong>IP:</strong> {{ company.company_ip }} </p>-->
        <p><strong>Theatre:</strong> {{ drama.theatre_name }}</p>
  </div>
  <el-row>
    <el-col :span="24">
      <el-table :data="tableData" stripe style="width: 70%;margin-top:  60px;margin-left: 230px ;">
        <el-table-column prop="drama_id" label="剧目号" width="200"/>
        <el-table-column prop="drama_name" label="剧目名" width="200"/>
<!--        <el-table-column prop="actor_id" label="演员序号" width="200"/>-->
        <el-table-column prop="actor_names" label="演员名" width="300"/>
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
.drama-profile {
  max-width: 300px;
  margin: 20px auto;
  padding: 20px;
  border: 1px solid #ccc;
  border-radius: 10px;
  text-align: center;
}

.custom-pagination{
  margin-left: 230px
}
</style>