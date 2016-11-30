import java.io.*;
import java.net.*;
import java.util.*;


public class makao_client
{
    
    public static String current = "";
    public static boolean yourturn = false;
    public static boolean hetes = false;
    public static boolean end = false;

    public static boolean isNumeric(String str)
    {
	try
	{
	  double d = Double.parseDouble(str);
	}
	catch(NumberFormatException nfe)
	{
	  return false;
	}
	return true;
    }
    
    public static boolean CheckCard(String card, String current_card)
    {
	String color = String.valueOf(card.charAt(0));
	String number = String.valueOf(card.charAt(1));
	String c_color = String.valueOf(current_card.charAt(0));
	String c_number = String.valueOf(current_card.charAt(1));
	
	if (current_card.indexOf('X') != -1)
	{
	    if (current_card.indexOf('X') == 0)
	    {
		if (number.equals(c_number))
		    return true;
		return false;
	    }
	    else
	    {
		if (color.equals(c_color))
		    return true;
		return false;
	    }
	}
	
	if (hetes)
	{
	    if (number.equals(c_number))
	    {
		return true;
	    }
	    return false;
	}
	
	if (color.equals(c_color) || number.equals(c_number))
	{
	    return true;
	}
	return false;
    }
    
    public static boolean CheckFor7(List<String> hand)
    {
	for (String n : hand)
	{
	    if (n.charAt(1) == '7')
	      return true;
	}
	return false;
    }
    
    public static boolean CheckRequestCard(String card)
    {
	if (card.equals("PF") || card.equals("MF") || card.equals("TF") || card.equals("ZF") ||
	card.equals("PA") || card.equals("MA") || card.equals("TA") || card.equals("ZA"))
	{
	    return true;
	}
	return false;
    }
    
    public static boolean CheckRequest(String card, String req)
    {
	if (card.charAt(1) == 'F')
	{
	    if (req.equals("PX") || req.equals("MX") || req.equals("TX") || req.equals("ZX"))
		return true;
	    
	    return false;
	}
	
	if (card.charAt(1) == 'A')
	{
	    if (req.equals("X7") || req.equals("X8") || req.equals("X9") || req.equals("X0") || req.equals("XA") || req.equals("XF") || req.equals("XK") || req.equals("XS"))
		return true;
	    
	    return false;
	}
	
	return false;
    }

    public static void DoCommand(String command, String arguments, List<String> hand, PrintWriter out)
    {
	
	if (command.equals("DRAW"))
	{
	    String[] cards = arguments.split(",");
	    for (int i=0; i<cards.length; i++)
	    {
		hand.add(cards[i]);
	    }
	    
	    return;
	}
	
	if (command.equals("TURN"))
	{
	    System.out.println();
	    System.out.println("Te jössz!");
	    yourturn = true;
	    System.out.println("A lapjaid: ");
	    
	    //Kártya sorszámok
	    for (int i=1; i<=hand.size(); i++)
	    {
		if (i < 10)
		    System.out.print(Integer.toString(i) + "  ");
		else
		    System.out.print(Integer.toString(i) + " ");
	    }
	    System.out.println();
	    //----------
	    
	    for (String card : hand)
	    {
		System.out.print(card + " ");
	    }
	    System.out.println();
	    
	    return;
	}
	
	if (command.equals("NOTURN"))
	{
	    yourturn = false;
	    
	    return;
	}
	
	if (command.equals("CURRENT"))
	{
	    current = arguments;
	    
	    return;
	}
	
	if (command.equals("REQUEST"))
	{
	    current = arguments;
	    
	    return;
	}
	
	if (command.equals("HETES"))
	{
	    System.out.print("Hetest kaptál! ");
	    hetes = CheckFor7(hand);
	    if (!hetes)
	    {
		System.out.println("Húzol " + arguments + " lapot.");
		out.println("DRAW " + arguments);
	    }
	    else
	    {
		System.out.println("De van neked is. Rakd le!");
	    }
	    
	    return;
	}
	
	if (command.equals("SKIP"))
	{
	    System.out.println("Ászt kaptál! Egy körből kimaradsz.");
	    out.println("SKIP");
	    
	    return;
	}
	
	if (command.equals("WIN"))
	{
	    end = true;
	    String[] arg = arguments.split(",");
	    if (arg[0].equals("0"))
		System.out.println("Vesztettél! A(z) " + Integer.toString(Integer.parseInt(arg[1])+1) + ". játékos nyert.");
	    else
		System.out.println("Nyertél!");
		
	    return;
	}
	
	if (command.equals("SURRWIN"))
	{
	    end = true;
	    System.out.println("Mindenki más feladta! Nyertél!");
	    
	    return;
	}
	
	if (command.equals("MESSAGE"))
	{
	    System.out.println(arguments);
	    
	    return;
	}
    }

    public static void main (String[] args) throws IOException
    {
    
	if (!isNumeric(args[0])) 
	{
	    System.err.println ("Nincs megadva/nem létezik iSlyen port!");
	    System.exit(1);
	}

        String command;
	String serverHostname = new String ("127.0.0.1");
	Socket socket = null;
	PrintWriter out = null;
	BufferedReader in = null;
	BufferedReader stdin = null;
    	
    	List<String> hand = new ArrayList<String>();

        System.out.println("Csatlakozás a szerverhez a " + serverHostname + " címen és " + args[0] + " porton...");

        try
        {
            socket = new Socket(serverHostname, Integer.parseInt(args[0]));
            out = new PrintWriter(socket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } 
        catch (UnknownHostException e) 
        {
            System.err.println ("Nem található a " + serverHostname + " cím.");
            System.exit(1);
        } 
        catch (IOException e) 
        {
            System.err.println("Nem sikerült csatlakozni a szerverhez.");
            System.exit(1);
        }
        
        stdin = new BufferedReader(new InputStreamReader(System.in));
        String[] input;

        while(!end)
        {
	    if (in.ready())
	    {
		String s = in.readLine();
		String[] cparts = s.split(" ", 2);
		/*if (cparts[0].equals("MESSAGE"))
		{
		    cparts = s.split(" ", 2);
		}*/
		DoCommand(cparts[0], cparts[1], hand, out);
	    }
	    
	    if (end) break;
	    
	    if (stdin.ready())
	    {
		input = stdin.readLine().split(" ");
		if (yourturn)
		{
		    if (input[0].equals("feladom"))
		    {
			end = true;
			out.println("SURRENDER");
		    }
		    else
		    {
			if (input[0].equals("lapot"))
			{
			    out.println("DRAW 1");
			}
			else
			{
			    if (isNumeric(input[0]))
			    {
				int n = Integer.parseInt(input[0]);
			    
				if (n > 0 && n <= hand.size())
				{
				    if (CheckCard(hand.get(n-1), current))
				    {
					if (CheckRequestCard(hand.get(n-1)))
					{
					    if (input.length > 1)
					    {
						if (CheckRequest(hand.get(n-1), input[1]))
						{
						    current = hand.get(n-1);
						    hand.remove(n-1);
						    if (hand.size() > 0)
							out.println("REQUEST " + current + " " + input[1]);
						    else
							out.println("WIN");
						    System.out.println();
						}
						else
						{
						    System.out.println("Rossz kérő parancs!");
						}
					    }
					    else
					    {
						System.out.println("Rossz kérő parancs!");
					    }
					}
					else
					{
					    current = hand.get(n-1);
					    hand.remove(n-1);
					    if (hand.size() > 0)
						out.println("CURRENT " + current);
					    else
						out.println("WIN");
					    System.out.println();
					}
				    }
				    else
				    {
					System.out.println("Ezt nem tudod lerakni!");
				    }
				}
				else
				{
				    System.out.println("Nincs ilyen lapod!");
				}
			    }
			    else
			    {
				System.out.println("Nincs ilyen parancs!");
			    }
			}
		    }
		}
		else
		{
		    System.out.println("Nem a te köröd van!");
		}
		
		hetes = false;
	    }
	}

	out.close();
        in.close();
        stdin.close();
        socket.close();
    }
}