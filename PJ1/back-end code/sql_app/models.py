from datetime import datetime

from sqlalchemy import Boolean, Column, Integer, String, ForeignKey,Table
from sqlalchemy.dialects.mysql import DATETIME

from .database import Base
from typing import List
from typing import Optional
from sqlalchemy import ForeignKey
from sqlalchemy import String
from sqlalchemy import create_engine
from sqlalchemy.orm import DeclarativeBase
from sqlalchemy.orm import Mapped
from sqlalchemy.orm import mapped_column
from sqlalchemy.orm import relationship

class Base(DeclarativeBase):
    pass

class User(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True)
    username = Column(String(20), unique=True, index=True)
    first_name = Column(String(50))
    last_name = Column(String(50))
    email = Column(String(50),nullable=True)
    hashed_password = Column(String(255))
    is_active = Column(Boolean, default=True)


class DramaActor(Base):
    __tablename__ = "drama_actor"
    drama_id = Column(Integer, ForeignKey("dramas.id"), primary_key=True)
    actor_id = Column(Integer, ForeignKey("actors.id"), primary_key=True)
    date = Column(DATETIME, nullable=True)

class Actor(Base):
    __tablename__ = "actors"

    id = Column(Integer, primary_key=True)
    actor_name = Column(String(100), unique=True, index=True)
    birth = Column(String(10))
    sex = Column(String(5))
    dramas: Mapped[List['Drama']] = relationship(secondary="drama_actor", back_populates="actors")
#
class Drama(Base):
    __tablename__ = "dramas"

    id = Column(Integer, primary_key=True)
    drama_name = Column(String(100), unique=True, index=True)
    theatre_name = Column(String(100), index=True)
    company_id = Column(Integer, ForeignKey("companies.id"))
    company = relationship("Company", back_populates="dramas")
    actors : Mapped[List[Actor]]= relationship( secondary="drama_actor", back_populates="dramas")
#
#
class Company(Base):
    __tablename__ = "companies"

    id = Column(Integer, primary_key=True)
    company_name = Column(String(100), unique=True, index=True)
    company_ip = Column(String(50), index=True)
    dramas: Mapped[List[Drama]] = relationship("Drama", back_populates="company")

