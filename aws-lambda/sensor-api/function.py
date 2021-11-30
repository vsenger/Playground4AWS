import boto3
import json

print('Loading function')


def handler(event, context):
	client = boto3.client('iot-data')
	dynamodb = boto3.client('dynamodb')
	action = event['queryStringParameters']['action']
	data = ""
	if action=="sensor":
		sensor = event['queryStringParameters']['sensor']
		#teste = dynamodb.scan(TableName='gaming_ai')
		data = dynamodb.query(
		    TableName='gaming_ai',
		    KeyConditionExpression='#name = :value',
		    ExpressionAttributeValues={
		      ':value': {
		        'S': sensor
		      }
		    },
		    ExpressionAttributeNames={
		      '#name': 'sensor'
		    },
	        Limit=1,
	        ScanIndexForward=False
		)	
		#print(event)
		#client.publish(topic=topicValue, payload=messageValue)
	if action=="mqtt":
		topicValue = event['queryStringParameters']['topic']
		messageValue = event['queryStringParameters']['message']
		#print(event)
		client.publish(topic=topicValue, payload=messageValue)
		data = { 'status' : 'sent', 'message' : messageValue, 'topic': topicValue }
	return {
       'statusCode': 200,
       'body': json.dumps(data)
	}

