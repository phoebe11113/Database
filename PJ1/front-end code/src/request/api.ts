import instance from "@/request/http";
import loginInstance from "@/request/http_login"

//一般情况下，接口类型会放到一个文件
// 下面两个TS接口，表示要传的参数
interface ReqLogin {
    username: string
    password: string
}
interface ResLogin {
    access_token: string
}

interface ReqRegister {
    username: string
    email: string
    first_name: string
    last_name: string
    password: string
}

interface ReqStatus {
    id: string
    navStatus: string
}


// Res是返回的参数，T是泛型，需要自己定义，返回对数统一管理***
type Res<T> = Promise<ItypeAPI<T>>;
// 一般情况下响应数据返回的这三个参数，
// 但不排除后端返回其它的可能性，
interface ItypeAPI<T> {
    success: string | null // 返回状态码的信息，如请求成功等;
    result: T,//请求的数据，用泛型
    msg: string | null // 返回状态码的信息，如请求成功等
    message:string
    code: number //返回后端自定义的200，404，500这种状态码
    user: User
    users: User[]
    drama: Drama
    dramas: Drama[]
    actor: Actor
    actors: Actor[]
    date: Date
    Dates: Date[]
    date_group: DateGroup
    Dates_group: DateGroup[]
    total_users: number
    total_pages: number
}

interface User {
    username: string
    email: string
    first_name: string
    last_name: string
    is_active: boolean
    // is_superuser: boolean
}
interface Drama{
    drama_name: string
    theatre_name: string
    company_id: int
}
interface DramaCreat{
    drama_name: string
    theatre_name: string
    company_id: int
}
interface Company{
    company_name: string
    company_ip: string
}
interface Actor{
    actor_name: string
    birth: string
    sex: string
}
interface Date{
    drama_id: int
    actor_id: int
    drama_name: string
    actor_name: string
    date: Date
}
interface DateGroup{
    drama_id: int
    drama_name: string
    actor_names: string[]
    date: Date
}
interface ID{
    drama_id: int
    actor_id: int
}
interface DateCreat{
    drama_id: int
    actor_id: int
    date: Date
}
interface UserList {
    total: int
    users: User[]
}
interface CompanyList{
    total: int
    companies: Company[]
}
interface DramaList {
    total: int
    dramas: Drama[]
}
interface ActorList {
    total: int
    actors: Actor[]
}
interface DateList{
    total: int
    dates:Date[]
}
interface DateListGroup{
    total: int
    dates_group: DateGroup[]
}
interface CompanyId{
    company_id: int
}
//测试hello api
export const TestHello = (): Res<null> =>
    instance.get('/api/hello');

//登录 api
export const LoginApi = (data: ReqLogin): ResLogin =>
    loginInstance.post('/api/token', data);

//注册 api
export const RegisterApi = (data: ReqRegister): User =>
    instance.post('/api/users/', data);

//添加演员
export const ActorApi = (data: Actor): Actor =>
    instance.post('/api/actors/', data);
//添加剧目
export const DramaApi = (data: DramaCreat): DramaCreat =>
    instance.post('/api/dramas/', data);
//添加排期
export const DateApi = (data: DateCreat): DateCreat =>
    instance.post('/api/dates/', data);
//添加排期
export const CompanyApi = (data: Company): Company =>
    instance.post('/api/companies/', data);

//登出 api
export const LogoutApi = (): Res<null> =>
    instance.get('/api/logout');

//根据username查询用户信息api  get
export const GetUserInfoByUserName = (params: { userName: string }): User =>
    instance.get(`/api/users/${params.userName}`);

export const GetCompanyInfoByCompanyId = (params: { company_id: int }): Company =>
    instance.get(`/api/company/${params.company_id}`);

export const GetUserInfoList = (params: { skip: number, limit: number }): UserList =>
    instance.get(`/api/users/`, {params});

export const GetDramaInfoByDramaName = (params: { drama_name: str }): Drama =>
    instance.get(`/api/drama/${params.drama_name}`);
export const GetIdByName = (params: { drama_name: string ,actor_name: string }): ID =>
    instance.get(`/api/dates_name/${params.drama_name}/${params.actor_name}`);
export const GetActorInfoByActorName = (params: { actor_name: str }): Actor =>
    instance.get(`/api/actor/${params.actor_name}`);
// export const GetDramaActorInfoListByDramaId = (params: { drama_id: int,skip: number, limit: number }): DateList =>
//     instance.get(`/api/date/${params.drama_id}`);
export const GetDramaActorInfoListByDramaName = (params: { drama_name: string,skip: number, limit: number }): Promise<DateListGroup> =>
    instance.get(`/api/dates/${params.drama_name}`);
export const GetDramaActorInfoListByActorName = (params: { actor_name: string,skip: number, limit: number }): DateList =>
    instance.get(`/api/dates_actor/${params.actor_name}`);


export const GetDramaInfoList = (params: { skip: number, limit: number }): DramaList =>
    instance.get(`/api/dramas/`, {params});
export const GetDramaIdByDramaName = (params: { drama_name: string }): Drama =>
    instance.get(`/api/drama_id/${params.drama_name}`);

export const GetDramaInfoListByCompany = (params: { skip: number, limit: number }): DramaList =>
    instance.get(`/api/dramas/${params.company_id}`);

export const GetActorInfoList = (params: { skip: number, limit: number }): ActorList =>
    instance.get(`/api/actors/`, {params});
export const GetCompanyInfoList = (params: { skip: number, limit: number }): CompanyList =>
    instance.get(`/api/companies/`, {params});

//根据pageNumber查询用户信息api  get
export const GetUserInfoByPageNum = (params: { pageNumber: number }): Res<null> =>
    instance.get(`/api/users/list/${params.pageNumber}`, {params});

export const GetCompanyIdByCompanyName = (params: { company_name: string }): int =>
    instance.get(`/api/company_id/${params.company_name}`)

//以下是模板:
// post请求 ，没参数
export const LogoutAPI = (): Res<null> =>
    instance.post("/admin/logout");

// post请求，有参数,如传用户名和密码
export const loginAPI = (data: ReqLogin): Res<string> =>
    instance.post("/admin/login", data);

// post请求 ，没参数，但要路径传参
export const StatusAPI = (data: ReqStatus): Res<null> =>
    instance.post(`/productCategory?ids=${data.id}&navStatus=${data.navStatus}`);


//  get请求，没参数，
export const FlashSessionListApi = (): Res<null> =>
    instance.get("/flashSession/list");

// get请求，有参数，路径也要传参  (也可能直接在这写类型，不过不建议,大点的项目会维护一麻烦)
export const ProductCategoryApi = (params: { parentId: number }): any =>
    instance.get(`/productCategory/list/${params.parentId}`, {params});

// get请求，有参数，(如果你不会写类型也可以使用any,不过不建议,因为用了之后 和没写TS一样)
export const AdminListAPI = (params: any): any =>
    instance.get("/admin/list", {params});
