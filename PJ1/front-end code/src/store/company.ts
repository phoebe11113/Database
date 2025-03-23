import {defineStore} from "pinia";

export const useCompanystore = defineStore(
    'company',
    {
        state() {
            return {
                company_id:'company_id',
                token:'token',
            }
        }
    }
)