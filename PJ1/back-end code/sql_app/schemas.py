from datetime import datetime

from pydantic import BaseModel
from typing import List


class UserBase(BaseModel):
    username: str
    first_name: str
    last_name: str
    email: str

class UserCreate(UserBase):
    password: str

class UsersOut(BaseModel):
    total: int
    users: List[UserBase]

class User(UserBase):
    id: int
    is_active: bool

    class Config:
        orm_mode = True

class ActorBase(BaseModel):
    actor_name: str
    birth: str
    sex: str

class ActorCreate(ActorBase):
    pass

class ActorsOut(BaseModel):
    total: int
    actors: List[ActorBase]

class Actor(ActorBase):
    id: int

    class Config:
        orm_mode = True

class ActorId(BaseModel):
    id: int

class DramaBase(BaseModel):
    drama_name: str
    theatre_name: str
    company_id: int

class DramaCreate(DramaBase):
    pass

class DramaOut(BaseModel):
    total: int
    dramas: List[DramaBase]

class Drama(DramaBase):
    id: int

    class Config:
        orm_mode = True

class DramaId(BaseModel):
    id: int

class DateBase(BaseModel):
    drama_id: int
    actor_id: int
    drama_name: str
    actor_name: str
    date: datetime

class DateGroup(BaseModel):
    drama_id: int
    drama_name: str
    actor_names: List[str]
    date: datetime

class DateCreate(BaseModel):
    drama_id: int
    actor_id: int
    date: datetime

class ID(BaseModel):
    drama_id: int
    actor_id: int

class DateOut(BaseModel):
    total: int
    dates: List[DateBase]

class DateOutGroup(BaseModel):
    total: int
    dates_group: List[DateGroup]

class Date(DateBase):
    date: datetime
    class Config:
        orm_mode = True

class CompanyBase(BaseModel):
    company_name: str
    company_ip: str

class CompanyCreate(CompanyBase):
    pass

class CompanyOut(BaseModel):
    total: int
    companies: List[CompanyBase]

class Company(CompanyBase):
    company_name: str

    class Config:
        orm_mode = True