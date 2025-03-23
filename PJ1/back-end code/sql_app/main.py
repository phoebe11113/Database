from fastapi import Depends, FastAPI, HTTPException, status

from .crud import get_user_by_username
import jwt
from typing import Annotated

from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from pydantic import BaseModel

from datetime import datetime, timedelta, timezone

from jwt.exceptions import InvalidTokenError
from passlib.context import CryptContext
from sqlalchemy.orm import Session

from . import crud, models, schemas
from .database import SessionLocal, engine
from passlib.context import CryptContext
from fastapi.routing import APIRoute

# to get a string like this run:
# openssl rand -hex 32
SECRET_KEY = "137d4e686d228592c12584d97c2bef60638701c521726150499c8e2f4b84aa9a"
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

# fake_users_db = {
#     "phoebe": {
#         "username": "phoebe",
#         "first_name": "yufei",
#         "last_name": "liu",
#         "email": "yufei@email.com",
#         "hashed_password": "9133002",
#         "is_active": True,
#     }}


class Token(BaseModel):
    access_token: str
    token_type: str


class TokenData(BaseModel):
    username: str | None = None


class User(BaseModel):
    id: int
    username: str
    email: str | None = None
    first_name: str | None = None
    last_name: str | None = None
    is_active: bool | None = None


class UserInDB(User):
    hashed_password: str


pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

models.Base.metadata.create_all(bind=engine)

app = FastAPI()

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

def verify_password(plain_password, hashed_password):
    return pwd_context.verify(plain_password, hashed_password)


def get_password_hash(password):
    return pwd_context.hash(password)


def get_user(db, username: str):
    if username in db:
        user_dict = db[username]
        return UserInDB(**user_dict)


def authenticate_user(fake_db, username: str, password: str):
    user = crud.get_user_by_username(fake_db, username)
    if not user:
        return False
    if not verify_password(password, user.hashed_password):
        return False
    return user


def create_access_token(data: dict, expires_delta: timedelta | None = None):
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.now(timezone.utc) + expires_delta
    else:
        expire = datetime.now(timezone.utc) + timedelta(minutes=15)
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt


async def get_current_user(token: Annotated[str, Depends(oauth2_scheme)],db: Session = Depends(get_db)):
    credentials_exception = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        username: str = payload.get("sub")
        if username is None:
            raise credentials_exception
        token_data = TokenData(username=username)
    except InvalidTokenError:
        raise credentials_exception
    user = get_user_by_username(db,username=token_data.username)
    if user is None:
        raise credentials_exception
    return user


async def get_current_active_user(
    current_user: Annotated[User, Depends(get_current_user)],
):
    if not current_user.is_active:
        raise HTTPException(status_code=400, detail="Inactive user")
    return current_user


@app.post("/token")
async def login_for_access_token(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()],db: Session = Depends(get_db)
) -> Token:
    user = authenticate_user(db, form_data.username, form_data.password)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Incorrect username or password",
            headers={"WWW-Authenticate": "Bearer"},
        )
    access_token_expires = timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    access_token = create_access_token(
        data={"sub": user.username}, expires_delta=access_token_expires
    )
    return Token(access_token=access_token, token_type="bearer")


@app.get("/users/me/", response_model=User)
async def read_users_me(
    current_user: Annotated[User, Depends(get_current_active_user)],
):
    return current_user


@app.get("/users/me/items/")
async def read_own_items(
    current_user: Annotated[User, Depends(get_current_active_user)],
):
    return [{"item_id": "Foo", "owner": current_user.username}]

# Dependency
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


@app.post("/users/", response_model=schemas.User)
def create_user(user: schemas.UserCreate, db: Session = Depends(get_db)):
    db_user = crud.get_user_by_username(db, username=user.username)
    if db_user:
        raise HTTPException(status_code=400, detail="Username already registered")
    return crud.create_user(db=db, user=user)
@app.post("/dates/", response_model=schemas.DateCreate)
def create_date(date: schemas.DateCreate, db: Session = Depends(get_db)):

    return crud.create_date(db=db, date=date)

@app.post("/actors/", response_model=schemas.Actor)
def create_actor(actor: schemas.ActorCreate, db: Session = Depends(get_db)):
    db_actor = crud.get_actor_by_actorname(db, actor_name=actor.actor_name)
    if db_actor:
        raise HTTPException(status_code=400, detail="Username already registered")
    return crud.create_actor(db=db, actor=actor)


@app.post("/dramas/", response_model=schemas.DramaCreate)
def create_drama(drama: schemas.DramaCreate, db: Session = Depends(get_db)):
    db_drama = crud.get_drama_by_dramaname(db, drama_name=drama.drama_name)
    if db_drama:
        raise HTTPException(status_code=400, detail="DramaName already registered")
    return crud.create_drama(db=db, drama = drama)

@app.post("/companies/", response_model=schemas.Company)
def create_company(company: schemas.CompanyCreate, db: Session = Depends(get_db)):
    db_company = crud.get_company_by_companyname(db, company_name=company.company_name)
    if db_company:
        raise HTTPException(status_code=400, detail="CompanyName already registered")
    return crud.create_company(db=db, company = company)


@app.get("/users/", response_model=schemas.UsersOut)
def read_users(
    skip: int = 0,
    limit: int = 100,
    db: Session = Depends(get_db),
    current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    users = crud.get_users(db, skip=skip, limit=limit)
    total = crud.get_users_number(db)
    return {
        'total':total,
        'users':users
    }

@app.get("/dramas/", response_model=schemas.DramaOut)
def read_dramas(
    skip: int = 0,
    limit: int = 100,
    db: Session = Depends(get_db),
    current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    dramas = crud.get_dramas(db, skip=skip, limit=limit)
    total = crud.get_dramas_number(db)
    return {
        'total':total,
        'dramas':dramas
    }

@app.get("/companies/", response_model=schemas.CompanyOut)
def read_companies(
    skip: int = 0,
    limit: int = 100,
    db: Session = Depends(get_db),
    current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    companies = crud.get_companies(db, skip=skip, limit=limit)
    total = crud.get_companies_number(db)
    return {
        'total':total,
        'companies':companies
    }

@app.get("/actors/", response_model=schemas.ActorsOut)
def read_actors(
    skip: int = 0,
    limit: int = 100,
    db: Session = Depends(get_db),
    current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    actors = crud.get_actors(db, skip=skip, limit=limit)
    total = crud.get_actors_number(db)
    return {
        'total':total,
        'actors':actors
    }



# @app.get("/users/{user_id}", response_model=schemas.User)
# def read_user(
#         user_id: int,
#         db: Session = Depends(get_db),
#         current_user: User = Depends(get_current_active_user)
#  ):
#     if not current_user:
#         raise HTTPException(status_code=403, detail="Forbidden")
#     db_user = crud.get_user(db, user_id=user_id)
#     if db_user is None:
#         raise HTTPException(status_code=404, detail="User not found")
#     return db_user

@app.get("/company/{company_id}", response_model=schemas.Company)
def read_company(
        company_id: int,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    db_company = crud.get_company(db, company_id=company_id)
    if db_company is None:
        raise HTTPException(status_code=404, detail="Company not found")
    return db_company

@app.get("/dramas/{company_id}", response_model=schemas.DramaOut)
def read_dramas(
        company_id: int,
        skip: int = 0,
        limit: int = 100,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    dramas_c = crud.get_drama_by_company(db, company_id=company_id,skip=skip, limit=limit)
    if dramas_c is None:
        raise HTTPException(status_code=404, detail="Drama not found")
    total = crud.get_dramas_number(db)
    return {
        'total':total,
        'dramas':dramas_c
    }
@app.get("/date/{drama_id}", response_model=schemas.DateOut)
def read_dates_id(
        drama_id: int,
        skip: int = 0,
        limit: int = 100,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    dates = crud.get_date_by_drama(db, drama_id=drama_id,skip=skip, limit=limit)
    if dates is None:
        raise HTTPException(status_code=404, detail="Date not found")
    total = crud.get_dates_number_by_drama(db, drama_id=drama_id)
    return {
        'total':total,
        'dates':dates
    }
@app.get("/dates/{drama_name}", response_model=schemas.DateOutGroup)
def read_dates_name(
        drama_name: str,
        skip: int = 0,
        limit: int = 100,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    dates_n = crud.get_date_with_names_by_drama_name(db, drama_name = drama_name,skip=skip, limit=limit)
    if dates_n is None:
        raise HTTPException(status_code=404, detail="Date not found")
    # total = crud.get_dates_number_by_drama_name(db, drama_name = drama_name)
    total = len(dates_n)
    return {
        'total':total,
        'dates_group':dates_n
    }
@app.get("/dates_actor/{actor_name}", response_model=schemas.DateOut)
def read_dates_actor_name(
        actor_name: str,
        skip: int = 0,
        limit: int = 100,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    dates_a = crud.get_date_with_names_by_actor_name(db, actor_name = actor_name,skip=skip, limit=limit)
    if dates_a is None:
        raise HTTPException(status_code=404, detail="Date not found")
    total = crud.get_dates_number_by_actor_name(db, actor_name = actor_name)
    return {
        'total':total,
        'dates':dates_a
    }

@app.get("/users/{user_name}", response_model=schemas.User)
def read_user(
        user_name: str,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    db_user = crud.get_user_by_username(db, username = user_name)
    if db_user is None:
        raise HTTPException(status_code=404, detail="User not found")
    return db_user

@app.get("/drama/{drama_name}", response_model=schemas.Drama)
def read_drama(
        drama_name: str,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    drama = crud.get_drama_by_dramaname(db, drama_name = drama_name)
    if drama is None:
        raise HTTPException(status_code=404, detail="Drama not found")
    return drama

@app.get("/actor/{actor_name}", response_model=schemas.Actor)
def read_actor(
        actor_name: str,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    actor = crud.get_actor_by_actorname(db, actor_name = actor_name)
    if actor is None:
        raise HTTPException(status_code=404, detail="Drama not found")
    return actor

@app.get("/dramas/{drama_id}", response_model=schemas.Drama)
def read_dramas_id(
        drama_id: int,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    db_drama = db.query(models.Drama).filter(models.Drama.id == drama_id).first()
    if db_drama is None:
        raise HTTPException(status_code=404, detail="Drama not found")
    return db_drama
@app.get("/dramas/{drama_id}", response_model=schemas.Drama)
def read_dramas_id(
        drama_id: int,
        db: Session = Depends(get_db),
        current_user: User = Depends(get_current_active_user)
 ):
    if not current_user:
        raise HTTPException(status_code=403, detail="Forbidden")
    db_drama = db.query(models.Drama).filter(models.Drama.id == drama_id).first()
    if db_drama is None:
        raise HTTPException(status_code=404, detail="Drama not found")
    return db_drama

# @app.get("/drama_id/{drama_name}", response_model=schemas.DramaId)
# def read_drama_id(
#         drama_name: str,
#         db: Session = Depends(get_db),
#  ):
#     drama_id = db.query(models.Drama.id).filter(models.Drama.drama_name == drama_name).first()
#     if drama_id is None:
#         raise HTTPException(status_code=404, detail="Drama not found")
#     return drama_id
# @app.get("/actor_id/{actor_name}", response_model=schemas.ActorId)
# def read_actor_id(
#         actor_name: str,
#         db: Session = Depends(get_db),
#  ):
#     actor_id = db.query(models.Actor.id).filter(models.Actor.actor_name == actor_name).first()
#     if actor_id is None:
#         raise HTTPException(status_code=404, detail="Drama not found")
#     return actor_id

@app.get("/company_id/{company_name}", response_model= int)
def read_company_id(
        company_name: str,
        db: Session = Depends(get_db),
 ):
    company_query = db.query(models.Company.id).filter(models.Company.company_name == company_name).first()
    company_id = company_query[0]
    if company_id is None:
        raise HTTPException(status_code=404, detail="Company not found")
    return company_id
# @app.get("/company_name/{company_id}", response_model= str)
# def read_company_name(
#         company_id: int,
#         db: Session = Depends(get_db),
#  ):
#     company_name = db.query(models.Company.company_name).filter(models.Company.id == company_id).first()
#
#     if company_name is None:
#         raise HTTPException(status_code=404, detail="Company not found")
#     return company_name

@app.get("/dates_name/{drama_name}/{actor_name}", response_model=schemas.ID)
def read_id(
        drama_name: str,
        actor_name: str,
        db: Session = Depends(get_db),
 ):
    drama_query = db.query(models.Drama.id).filter(models.Drama.drama_name == drama_name).first()
    drama_id = drama_query[0]
    if drama_id  is None:
        raise HTTPException(status_code=404, detail="Drama not found")
    actor_query = db.query(models.Actor.id).filter(models.Actor.actor_name == actor_name).first()
    actor_id = actor_query[0]
    if actor_id  is None:
        raise HTTPException(status_code=404, detail="Actor not found")
    return {
        'drama_id': drama_id,
        'actor_id': actor_id
    }

# for route in app.router.routes:
#     if isinstance(route, APIRoute):
#         print(f"Path: {route.path}, Method: {route.methods}, Name: {route.name}")

