//Test script

void main()
{
	int i = 0;
	Character c;
	c.name("WoLf");
	c.say("Hello World!");
	while(!c.isdead())
	{
		if(i++ % 3 == 0)
		{
			c.heal();
		}
		c.hit();
	}
}
