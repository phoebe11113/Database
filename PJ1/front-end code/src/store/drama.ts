import {defineStore} from "pinia";

export const useDramastore = defineStore(
    'drama',
    {
        state() {
            return {
                drama_name:'drama_name',
                token:'token',
            }
        }
    }
)