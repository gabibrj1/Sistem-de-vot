from django.urls import path
from . import views

urlpatterns = [
    path('session/start/', views.start_new_session),
    path('session/stop/', views.stop_session),
    path('session/restart/', views.restart_session),
    path('session/resume/', views.resume_session),  # nou
    path('session/current/', views.current_session),
    path('tokens/', views.create_token),
    path('votes/', views.create_vote),
    path('results/', views.results),
]
