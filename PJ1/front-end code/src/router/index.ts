import {createRouter, createWebHistory} from "vue-router";

import Login from '@/pages/Login.vue';
import Index from "@/pages/Index.vue";
import Test from "@/pages/Test.vue";
// import IndexMain from "@/components/IndexMain.vue";
import CheckUserInfo from "@/components/CheckUserInfo.vue";
import Profile from "@/components/Profile.vue";
import AddUser from "@/components/AddUser.vue";
import Register from "@/pages/Register.vue";
import CheckDramaInfo from "@/components/CheckDramaInfo.vue";
import checkActorInfo from "@/components/CheckActorInfo.vue";
import CompanyPlus from "@/components/CompanyPlus.vue";
import DramaPlus from "@/components/DramaPlus.vue";
import ActorPlus from "@/components/ActorPlus.vue";
import details from "@/pages/details.vue";
import AddActor from "@/components/AddActor.vue";
import AddDrama from "@/components/AddDrama.vue";
import AddCompany from "@/components/AddCompany.vue";
import AddDate from "@/components/AddDate.vue";
import CheckCompanyInfo from "@/components/CheckCompanyInfo.vue";

const routes =
    [
        {
            path: '/',
            name: 'Login',
            component: Login
        },
        {
            path: '/register',
            name: 'Register',
            component: Register
        },
        {
            path: '/index',
            name: 'Index',
            component: Index,
            children: [
                // {
                //     path: '',
                //     name:'IndexMain',
                //     component: IndexMain,
                // },
                {
                    path: '',
                    name:'IndexMain',
                    component: Profile,
                },
                {
                    path: 'checkUserInfo/:username',
                    name: 'checkUserDetail',
                    component: Profile,
                    props: true
                },
                {
                    path: 'checkUserInfo',
                    component: CheckUserInfo,
                },
                {
                    path: 'checkDramaInfo',
                    component: CheckDramaInfo,
                },

                // {
                //     path: 'checkDramaInfo/:company_id',
                //     name: 'checkCompanyDetail',
                //     component: CheckDramaByCompany,
                //     props: true
                // },
                {
                    path: 'checkDramaInfo/:company_id',
                    name: 'checkCompanyDetail',
                    component: CompanyPlus,
                    props: true
                },
                {
                    path: 'checkDramaInfo/:drama_name',
                    name: 'checkDramaDetail',
                    component: DramaPlus,
                    props: true
                },
                // {
                //     path: 'checkDramaInfo/:company_id',
                //     name: 'checkCompanyDetail',
                //     component: Company,
                //     props: true
                // },
                {
                    path: 'checkActorInfo',
                    component: checkActorInfo,
                },
                {
                    path: 'checkActorInfo/:actor_name',
                    name: 'checkActorDetail',
                    component: ActorPlus,
                    props: true
                },
                // {
                //     path: 'checkCompanyInfo',
                //     component: CheckCompanyInfo,
                // },
                // {
                //     path: 'checkCompanyInfo/:company_name',
                //     name: 'checkCompanyDetail',
                //     component: CompanyPlus,
                //     props: true
                // },
                {
                    path: 'addUser',
                    component: details,
                },
                {
                    path: 'addDrama',
                    component: AddDrama,
                },
                {
                    path: 'addActor',
                    component: AddActor,
                },
                {
                    path: 'addCompany',
                    component: AddCompany,
                },
                {
                    path: 'addDate',
                    component: AddDate,
                },
            ]

        },
        {
            path: '/test',
            name: 'Test',
            component: Test
        }
    ];

const router = createRouter({
    history: createWebHistory(),
    routes
});

export default router;
