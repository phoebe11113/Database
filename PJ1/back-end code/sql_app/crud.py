from dns.e164 import query
from sqlalchemy.orm import Session

from sql_app import models, schemas

from passlib.context import CryptContext
#创建一个加密对象
from passlib.context import CryptContext
from sqlalchemy import func

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

def get_password_hash(password):
    return pwd_context.hash(password)

def get_user(db: Session, user_id: int):
    return db.query(models.User).filter(models.User.id == user_id).first()
def get_company(db: Session, company_id: int):
    return db.query(models.Company).filter(models.Company.id == company_id).first()


def get_user_by_username(db: Session, username: str):
    return db.query(models.User).filter(models.User.username == username).first()

def get_actor_by_actorname(db: Session, actor_name: str):
    return db.query(models.Actor).filter(models.Actor.actor_name == actor_name).first()

def get_drama_by_dramaname(db: Session, drama_name: str):
    return db.query(models.Drama).filter(models.Drama.drama_name == drama_name).first()

def get_company_by_companyname(db: Session, company_name: str):
    return db.query(models.Company).filter(models.Company.company_name == company_name).first()

def get_drama_by_company(db: Session, company_id: int ,skip: int = 0, limit: int = 100):
    return db.query(models.Drama).filter(models.Drama.company_id == company_id).offset(skip).limit(limit).all()
def get_date_by_drama(db: Session, drama_id: int ,skip: int = 0, limit: int = 100):
    return db.query(models.DramaActor).filter(models.DramaActor.drama_id == drama_id).offset(skip).limit(limit).all()

def get_date_by_drama_name(db: Session, drama_name: str ,skip: int = 0, limit: int = 100):
    return db.query(models.DramaActor).join(models.Drama, models.DramaActor.drama_id == models.Drama.id).filter(models.Drama.drama_name == drama_name).offset(skip).limit(limit).all()


def get_date_with_names_by_drama_name(db: Session, drama_name: str, skip: int = 0, limit: int = 100):
    drama = db.query(models.Drama).filter(models.Drama.drama_name == drama_name).first()
    if not drama:
        return []  # 或者抛出异常，取决于您的业务逻辑

    # query = (
    #     db.query(models.DramaActor.drama_id, models.DramaActor.actor_id, models.Drama.drama_name.label('drama_name'), models.Actor.actor_name.label('actor_name'),models.DramaActor.date)
    #     .join(models.Drama, models.DramaActor.drama_id == models.Drama.id)
    #     .join(models.Actor, models.DramaActor.actor_id == models.Actor.id)
    #     .filter(models.Drama.drama_name == drama_name)
    #     .offset(skip)
    #     .limit(limit)
    # )
    #
    # dates_with_names = query.all()
    # return [{'drama_id': d.drama_id, 'actor_id': d.actor_id, 'drama_name': d.drama_name, 'actor_name': d.actor_name,'date':d.date} for
    #         d in dates_with_names]

    query = (
        db.query(
            models.DramaActor.drama_id,
            models.Drama.drama_name.label('drama_name'),
            models.DramaActor.date,
            func.group_concat(models.Actor.actor_name).label('actor_names')  # 使用 group_concat 来合并演员名称
        )
        .join(models.Drama, models.DramaActor.drama_id == models.Drama.id)
        .join(models.Actor, models.DramaActor.actor_id == models.Actor.id)
        .filter(models.Drama.drama_name == drama_name)
        .group_by(models.DramaActor.drama_id, models.DramaActor.date)  # 按剧目ID、剧目名称和日期分组
        .offset(skip)
        .limit(limit)
    )

    # 执行查询并构建结果列表
    results = query.all()
    return [
        {'drama_id': r.drama_id, 'drama_name': r.drama_name, 'actor_names': r.actor_names.split(','), 'date': r.date}
        for r in results]

def get_date_with_names_by_actor_name(db: Session, actor_name: str, skip: int = 0, limit: int = 100):
    actor = db.query(models.Actor).filter(models.Actor.actor_name == actor_name).first()
    if not actor:
        return ['error']  # 或者抛出异常，取决于您的业务逻辑

    query = (
        db.query(models.DramaActor.drama_id, models.DramaActor.actor_id, models.Drama.drama_name.label('drama_name'), models.Actor.actor_name.label('actor_name'),models.DramaActor.date)
        .join(models.Actor, models.DramaActor.actor_id == models.Actor.id)
        .join(models.Drama, models.DramaActor.drama_id == models.Drama.id)
        .filter(models.Actor.actor_name == actor_name)
        .offset(skip)
        .limit(limit)
    )

    dates_with_names = query.all()
    return [{'drama_id': d.drama_id, 'actor_id': d.actor_id, 'drama_name': d.drama_name, 'actor_name': d.actor_name,'date':d.date} for
            d in dates_with_names]




def get_users(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.User).offset(skip).limit(limit).all()
def get_dramas(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.Drama).offset(skip).limit(limit).all()
def get_actors(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.Actor).offset(skip).limit(limit).all()
def get_companies(db: Session, skip: int = 0, limit: int = 100):
    return db.query(models.Company).offset(skip).limit(limit).all()

def get_users_number(db: Session):
    return db.query(func.count(models.User.id)).scalar()
def get_companies_number(db: Session):
    return db.query(func.count(models.Company.id)).scalar()


def get_dramas_number(db: Session):
    return db.query(func.count(models.Drama.id)).scalar()
def get_dramas_number_by_company(db: Session, company_id: int):
    return db.query(func.count(models.Drama.id)).filter(models.Drama.company_id == company_id).scalar()
def get_dates_number_by_drama(db: Session, drama_id: int):
    return db.query(func.count(models.DramaActor.drama_id)).filter(models.DramaActor.drama_id == drama_id).scalar()

def get_dates_number_by_drama_name(db: Session, drama_name: str):
        query = db.query(
            func.count(models.DramaActor.drama_id)
        ).join(
            models.Drama,
            models.DramaActor.drama_id == models.Drama.id
        ).filter(
            models.Drama.drama_name == drama_name
        ).group_by(
            models.DramaActor.drama_id,
            models.DramaActor.date
        )
        return query.scalar()

def get_dates_number_by_actor_name(db: Session, actor_name: str):
    return db.query(func.count(models.DramaActor.drama_id)).join(models.Actor, models.DramaActor.actor_id == models.Actor.id).filter(models.Actor.actor_name == actor_name).scalar()

def get_actors_number(db: Session):
    return db.query(func.count(models.Actor.id)).scalar()






def create_user(db: Session, user: schemas.UserCreate):
    # fake_hashed_password = user.password
    fake_hashed_password = get_password_hash(user.password)
    db_user = models.User(username=user.username, hashed_password=fake_hashed_password, first_name=user.first_name, last_name=user.last_name,email=user.email)
    db.add(db_user)
    db.commit()
    db.refresh(db_user)
    return db_user

def create_date(db: Session, date: schemas.DateCreate):
    db_date = models.DramaActor(drama_id=date.drama_id,actor_id=date.actor_id,date = date.date )
    db.add(db_date)
    db.commit()
    db.refresh(db_date)
    return db_date

def create_actor(db: Session, actor: schemas.ActorCreate):
    db_actor = models.Actor(actor_name = actor.actor_name,birth = actor.birth,sex = actor.sex)
    db.add(db_actor)
    db.commit()
    db.refresh(db_actor)
    return db_actor

def create_drama(db: Session, drama: schemas.DramaCreate):
    db_drama = models.Drama(drama_name = drama.drama_name,theatre_name=drama.theatre_name,company_id=drama.company_id)
    db.add(db_drama)
    db.commit()
    db.refresh(db_drama)
    return db_drama

def create_company(db: Session, company: schemas.CompanyCreate):
    db_company = models.Company(company_name = company.company_name,company_ip= company.company_ip)
    db.add(db_company)
    db.commit()
    db.refresh(db_company)
    return db_company
