#version 330                                                        

// Interpolated shader
                                                      
in vec4 color;												
out vec4 FragColor;                         

uniform vec4 uniColor;
                                                                   
void main()                                                         
{                                                                    
    FragColor = uniColor;
}