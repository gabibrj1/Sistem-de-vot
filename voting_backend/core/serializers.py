from rest_framework import serializers
from .models import Session, Token, Vote


class SessionSerializer(serializers.ModelSerializer):
    class Meta:
        model = Session
        fields = ['id', 'name', 'started_at', 'active']


class TokenSerializer(serializers.ModelSerializer):
    class Meta:
        model = Token
        fields = ['id', 'value', 'user_id', 'created_at', 'used']


class CreateTokenSerializer(serializers.Serializer):
    user_id = serializers.CharField(max_length=64)

    def validate(self, attrs):
        user_id = attrs['user_id']
        try:
            session = Session.objects.get(active=True)
        except Session.DoesNotExist:
            raise serializers.ValidationError('Nu există sesiune activă')

        if Token.objects.filter(session=session, user_id=user_id, used=True).exists():
            raise serializers.ValidationError('Utilizatorul a votat deja în această sesiune')

        attrs['session'] = session
        return attrs

    def create(self, validated_data):
        from secrets import token_hex
        session = validated_data['session']
        user_id = validated_data['user_id']
        token, _ = Token.objects.get_or_create(
            session=session,
            user_id=user_id,
            defaults={'value': token_hex(8)}
        )
        return token


class VoteSerializer(serializers.ModelSerializer):
    token_value = serializers.CharField(write_only=True)
    user_id = serializers.CharField(write_only=True)

    class Meta:
        model = Vote
        fields = ['id', 'option', 'created_at', 'token_value', 'user_id']

    def validate(self, attrs):
        token_value = attrs['token_value']
        user_id = attrs['user_id']

        try:
            session = Session.objects.get(active=True)
        except Session.DoesNotExist:
            raise serializers.ValidationError('Nu există sesiune activă')

        try:
            token = Token.objects.get(
                session=session,
                value=token_value,
                user_id=user_id
            )
        except Token.DoesNotExist:
            raise serializers.ValidationError('Token invalid pentru acest utilizator sau sesiune')

        if token.used:
            raise serializers.ValidationError('Token deja folosit')

        if Vote.objects.filter(session=session, token__user_id=user_id).exists():
            raise serializers.ValidationError('Utilizatorul a votat deja în această sesiune')

        attrs['session'] = session
        attrs['token'] = token
        return attrs

    def create(self, validated_data):
        validated_data.pop('token_value')
        validated_data.pop('user_id')
        session = validated_data.pop('session')
        token = validated_data.pop('token')

        vote = Vote.objects.create(session=session, token=token, **validated_data)
        token.used = True
        token.save()
        return vote
