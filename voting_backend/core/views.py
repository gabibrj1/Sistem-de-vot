from django.db.models import Count
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status

from .models import Session, Token, Vote
from .serializers import (
    SessionSerializer,
    TokenSerializer,
    CreateTokenSerializer,
    VoteSerializer,
)

@api_view(['POST'])
def resume_session(request):
    """
    Repornește ultima sesiune inactivă (fără a crea una nouă).
    """
    # dacă există deja activă, nu facem nimic
    if Session.objects.filter(active=True).exists():
        session = Session.objects.get(active=True)
        serializer = SessionSerializer(session)
        return Response(serializer.data)

    # luăm ultima sesiune creată (după started_at)
    session = Session.objects.order_by('-started_at').first()
    if not session:
        return Response({'detail': 'Nu există nicio sesiune anterioară de reluat'}, status=status.HTTP_404_NOT_FOUND)

    session.active = True
    session.save()
    serializer = SessionSerializer(session)
    return Response(serializer.data)


@api_view(['POST'])
def start_new_session(request):
    """
    Pornește o sesiune nouă.
    NU șterge datele vechi, doar dezactivează sesiunile anterioare.
    Body: { "name": "Sesiune X" }
    """
    Session.objects.filter(active=True).update(active=False)
    name = request.data.get('name') or 'Sesiune fără nume'
    session = Session.objects.create(name=name, active=True)
    serializer = SessionSerializer(session)
    return Response(serializer.data, status=status.HTTP_201_CREATED)


@api_view(['POST'])
def restart_session(request):
    """
    Oprește orice sesiune activă, șterge toate token-urile și voturile,
    apoi pornește o sesiune nouă cu numele dat.
    Body: { "name": "Sesiune nouă" }
    """
    Session.objects.filter(active=True).update(active=False)
    Vote.objects.all().delete()
    Token.objects.all().delete()

    name = request.data.get('name') or 'Sesiune restartată'
    session = Session.objects.create(name=name, active=True)
    serializer = SessionSerializer(session)
    return Response(serializer.data, status=status.HTTP_201_CREATED)


@api_view(['POST'])
def stop_session(request):
    """
    Dezactivează sesiunea curentă, fără să șteargă voturile.
    """
    try:
        session = Session.objects.get(active=True)
    except Session.DoesNotExist:
        return Response({'detail': 'Nu există sesiune activă'}, status=status.HTTP_404_NOT_FOUND)

    session.active = False
    session.save()
    serializer = SessionSerializer(session)
    return Response(serializer.data)


@api_view(['GET'])
def current_session(request):
    try:
        session = Session.objects.get(active=True)
    except Session.DoesNotExist:
        return Response({'detail': 'Nu există sesiune activă'}, status=status.HTTP_404_NOT_FOUND)
    serializer = SessionSerializer(session)
    return Response(serializer.data)


@api_view(['POST'])
def create_token(request):
    serializer = CreateTokenSerializer(data=request.data)
    if serializer.is_valid():
        token = serializer.save()
        out = TokenSerializer(token)
        return Response(out.data, status=status.HTTP_201_CREATED)
    return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


@api_view(['POST'])
def create_vote(request):
    serializer = VoteSerializer(data=request.data)
    if serializer.is_valid():
        vote = serializer.save()
        return Response({'status': 'OK', 'id': vote.id}, status=status.HTTP_201_CREATED)
    return Response({'status': 'ERR', 'errors': serializer.errors}, status=status.HTTP_400_BAD_REQUEST)


@api_view(['GET'])
def results(request):
    """
    Dacă nu există sesiune activă, returnează 0 pentru toate opțiunile,
    dar NU dă eroare – pentru UI e mai simplu.
    """
    try:
        session = Session.objects.get(active=True)
    except Session.DoesNotExist:
        return Response({'A': 0, 'B': 0, 'C': 0})

    counts = (
        Vote.objects.filter(session=session)
        .values('option')
        .annotate(total=Count('id'))
    )
    data = {row['option']: row['total'] for row in counts}
    for opt in ['A', 'B', 'C']:
        data.setdefault(opt, 0)
    return Response(data)
