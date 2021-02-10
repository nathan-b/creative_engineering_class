#!/usr/bin/python2
from __future__ import print_function
import datetime
import pickle
import os.path
import rfc3339
import iso8601
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request

# If modifying these scopes, delete the file token.pickle.
SCOPES = ['https://www.googleapis.com/auth/calendar.readonly']

cred_types = ['work', 'personal']
services = []
calevents = []

def get_date_object(date_string):
  return iso8601.parse_date(date_string)

def get_date_string(date_object):
  return rfc3339.rfc3339(date_object)

def add_events(events):
    for event in events:
        new_start = event['start'].get('dateTime')
        if not new_start:
            continue
        added = False
        for i in range(0, len(calevents)):
            ev_start = calevents[i]['start'].get('dateTime')
            if new_start < ev_start:
                calevents.insert(i, event)
                added = True
                break
        if not added:
            calevents.append(event)

def build_schedule():
    alarm_events = []
    for i in range(1, len(calevents)):
        curr_start = get_date_object(calevents[i]['start'].get('dateTime'))
        prev_end = get_date_object(calevents[i - 1]['end'].get('dateTime'))
        td = curr_start - prev_end
        for j in xrange(300, int(td.total_seconds()), 3600):
            stime = prev_end + datetime.timedelta(seconds=j)
            if (stime.hour > 21) or (stime.hour < 9): # No alarms after 9pm or before 9am
                continue
            alarm_events.append({'summary': 'Alarm!', 'start': {'dateTime': get_date_string(stime)}, 'end': {'dateTime': get_date_string(stime + datetime.timedelta(seconds=5))}})
    add_events(alarm_events)

def main():
    for ctype in cred_types:
        creds = None
        ptoken_name = ctype + '_token.pickle'
        # The file token.pickle stores the user's access and refresh tokens, and is
        # created automatically when the authorization flow completes for the first
        # time.
        if os.path.exists(ptoken_name):
            with open(ptoken_name, 'rb') as token:
                creds = pickle.load(token)
        # If there are no (valid) credentials available, let the user log in.
        if not creds or not creds.valid:
            if creds and creds.expired and creds.refresh_token:
                creds.refresh(Request())
            else:
                flow = InstalledAppFlow.from_client_secrets_file('work_credentials.json', SCOPES)
                creds = flow.run_local_server(port=0)
            # Save the credentials for the next run
            with open(ptoken_name, 'wb') as token:
                pickle.dump(creds, token)

        service = build('calendar', 'v3', credentials=creds)
        services.append(service)

    # Call the Calendar API
    now = datetime.datetime.utcnow().isoformat() + 'Z' # 'Z' indicates UTC time
    print('Getting the upcoming 10 events for each calendar')
    for service in services:
        events_result = service.events().list(calendarId='primary',
                                              timeMin=now,
                                              maxResults=10,
                                              singleEvents=True,
                                              orderBy='startTime').execute()
        events = events_result.get('items', [])
        add_events(events)

    build_schedule()

    if not calevents:
        print('No upcoming events found.')
    for event in calevents:
        start = event['start'].get('dateTime', event['start'].get('date'))
        end = event['end'].get('dateTime', event['end'].get('date'))
        print(start, '-', end, event['summary'])


if __name__ == '__main__':
    main()

